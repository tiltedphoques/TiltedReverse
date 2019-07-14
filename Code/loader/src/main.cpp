#include <windows.h>
#include <Platform.h>
#include <SteamLauncher.h>
#include <MessageBox.h>
#include <Config.h>

#include <string>
#include <iostream>
#include <locale>
#include <codecvt>
#include <fstream>
#include <filesystem>

#ifdef PUBLIC_BUILD
constexpr auto s_dllKey = "DllRelease";
#else
constexpr auto s_dllKey = "DllDebug";
#endif

#ifdef _WIN64
constexpr auto s_gameIdKey = "GameId64";
constexpr auto s_gameExeKey = "GamePath64";
#else
constexpr auto s_gameIdKey = "GameId32";
constexpr auto s_gameExeKey = "GamePath32";
#endif

bool ExecuteFunctionInProcess(HANDLE aProcessHandle, const std::wstring& acModuleName, const std::string& acFunctionName, const std::string& acParameter)
{
    auto showErrorMessage = [&aProcessHandle](auto&& ... args)
    {
        ErrorMessageBox messageBox;
        ((messageBox << args), ...);

        messageBox.Show();
    };

    auto bytesToWrite = (acParameter.size() + 1) * sizeof(char);

    auto pBaseAddress = VirtualAllocEx(aProcessHandle, nullptr, bytesToWrite, MEM_COMMIT, PAGE_READWRITE);
    if (!pBaseAddress)
    {
        showErrorMessage(L"Couldn't allocate ", bytesToWrite, L" bytes in the process.");
        return false;
    }

    size_t bytesWritten = 0;
    if (!WriteProcessMemory(aProcessHandle, pBaseAddress, acParameter.c_str(), bytesToWrite, reinterpret_cast<SIZE_T*>(&bytesWritten)) || bytesToWrite != bytesToWrite)
    {
        showErrorMessage(L"Failed to write ", bytesToWrite, L" in process memory, ", bytesWritten, L" bytes were written.");
        return false;
    }

    auto moduleHandle = GetModuleHandle(acModuleName.c_str());
    if (!moduleHandle)
    {
        showErrorMessage(L"Couldn't find module ", std::quoted(acModuleName), L".");
        return false;
    }

    auto funcAddress = GetProcAddress(moduleHandle, acFunctionName.c_str());
    if (!funcAddress)
    {
        size_t charConverted;
        auto message = acFunctionName;

        std::vector<wchar_t> pConvertedString(message.size() + 1);
        mbstowcs_s(&charConverted, pConvertedString.data(), pConvertedString.size(), message.c_str(), message.size());

        showErrorMessage(L"Couldn't get address for ", std::quoted(pConvertedString.data()), L" from ", std::quoted(acModuleName), L".");
        return false;
    }

    auto thread = CreateRemoteThread(aProcessHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(funcAddress), pBaseAddress, 0, nullptr);
    if (!thread)
    {
        showErrorMessage(L"Couldn't create a thread in the process.");
        return false;
    }

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    VirtualFreeEx(aProcessHandle, pBaseAddress, 0, MEM_RELEASE);
    return true;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    TP_UNUSED(hInstance);
    TP_UNUSED(hPrevInstance);
    TP_UNUSED(nCmdShow);

    bool skipSteamCheck = false;
    wchar_t** argList;
    int argCount;

    argList = CommandLineToArgvW(GetCommandLine(), &argCount);
    if (argList != nullptr)
    {
        for (int i = 0; i < argCount; i++)
        {
            std::wstring_view arg = argList[i];
            if (arg == L"--skip-steam")
            {
                std::wcout << L"Requested skip of Steam verification, this might cause issues." << std::endl;
                skipSteamCheck = true;
            }
        }
        LocalFree(argList);
    }

    Config config("tp_loader.conf");

    auto gameId = config.Get(s_gameIdKey);
    auto gameExe = config.Get(s_gameExeKey);
    auto dllName = config.Get(s_dllKey);

    if (GetEnvironmentVariable(L"STEAM_COMPAT_DATA_PATH", nullptr, 0) > 0 || GetLastError() != ERROR_ENVVAR_NOT_FOUND)
    {
        std::ofstream ofs("steam_appid.txt");
        ofs << gameId << std::endl;
        ofs.close();

        skipSteamCheck = true;
    }

    if (!skipSteamCheck)
    {
        // Launch Steam if it is not running.
        SteamLauncher launcher;
    }

    auto currentDir = std::filesystem::current_path();
    auto exePath = currentDir / gameExe;

    STARTUPINFO startupInfo = { 0 };
    PROCESS_INFORMATION processInfo = { 0 };

    SetEnvironmentVariableA("SteamGameId", gameId.c_str());
    SetEnvironmentVariableA("SteamAppId", gameId.c_str());

    if (!CreateProcess(exePath.c_str(), pCmdLine, nullptr, nullptr, false, CREATE_SUSPENDED, nullptr, nullptr, &startupInfo, &processInfo))
    {
        ErrorMessageBox messageBox;
        messageBox << L"An error occured when launching " << exePath << L".";

        messageBox.Show();
        return EXIT_FAILURE;
    }

    auto dllPath = currentDir / dllName;

    uint32_t exitCode = EXIT_FAILURE;

    // Inject our DLL.
    if (ExecuteFunctionInProcess(processInfo.hProcess, L"kernel32.dll", "LoadLibraryA", dllPath.string()))
    {
        // Resume the game's process.
        ResumeThread(processInfo.hThread);

        // Wait for the process to terminate.
        WaitForSingleObject(processInfo.hProcess, INFINITE);

        // Get game's exit code.
        GetExitCodeProcess(processInfo.hProcess, reinterpret_cast<LPDWORD>(&exitCode));
    }

    // Make sure it is dead.
    TerminateProcess(processInfo.hProcess, EXIT_FAILURE);

    // Clean up.
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return exitCode;
}
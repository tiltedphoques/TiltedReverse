#include <windows.h>

#include <SteamLauncher.hpp>
#include <Registry.hpp>

#include <filesystem>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

SteamLauncher::SteamLauncher()
{
    if (!IsRunning())
    {
        Launch();
    }
}

bool SteamLauncher::IsRunning() const
{
    return Registry::Read<uint32_t>(Registry::Key::CurrentUser, L"SOFTWARE", L"Valve", L"Steam", L"ActiveProcess", L"ActiveUser") > 0;
}

void SteamLauncher::Launch() const
{
    const std::filesystem::path installPath = Registry::Read<std::wstring>(Registry::Key::CurrentUser, L"SOFTWARE", L"Valve", L"Steam", L"SteamPath");
    const std::filesystem::path exePath = Registry::Read<std::wstring>(Registry::Key::CurrentUser, L"SOFTWARE", L"Valve", L"Steam", L"SteamExe");

    STARTUPINFO startupInfo = { 0 };
    PROCESS_INFORMATION processInfo = { 0 };

    if (!CreateProcess(exePath.c_str(), nullptr, nullptr, nullptr, false, 0, nullptr, installPath.c_str(), &startupInfo, &processInfo))
    {
        ErrorMessageBox messageBox;
        messageBox << L"An error occured when launching " << exePath << L".";

        messageBox.Show();
        ExitProcess(EXIT_FAILURE);
    }

    // Wait for Steam to start up.
    do
    {
        std::this_thread::sleep_for(1s);
    } while (!IsRunning());

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}

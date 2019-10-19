#include <Windows.h>
#include <Config.hpp>
#include <filesystem>


void LoadComplexLibrary(HMODULE aModule)
{
    wchar_t szPath[MAX_PATH];

    if (!GetModuleFileNameW(aModule, szPath, MAX_PATH))
        return;

    const auto currentDir = std::filesystem::path(szPath).parent_path();
    const auto configPath = currentDir / "tp_loader.conf";

    const Config config(configPath.string());

    const auto gameId = config.Get(s_gameIdKey);
    const auto gameExe = config.Get(s_gameExeKey);
    const auto dllName = config.Get(s_dllKey);
    const auto dllPath = currentDir / dllName;

    AddDllDirectory(currentDir.wstring().c_str());
    LoadLibraryExW(dllPath.wstring().c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    (void)lpReserved;

    switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        LoadComplexLibrary(hModule);
        break;
    default:
        break;
    }

    return TRUE;
}

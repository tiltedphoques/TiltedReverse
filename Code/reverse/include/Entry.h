#pragma once

#include <Platform.h>
#include <memory>

struct App;
void RegisterApp(std::unique_ptr<App> aApp);

#if TP_PLATFORM_64

using TGetWinmain = void*(__stdcall*)();
extern TGetWinmain OriginalGetWinmain;

void* GetWinmainHook();

#define DEFINE_DLL_ENTRY_INITIALIZER(className) \
    BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)\
{\
switch (fdwReason)\
{\
case DLL_PROCESS_ATTACH:\
{\
    RegisterApp(std::make_unique<className>());\
    OriginalGetWinmain = (TGetWinmain)GetProcAddress(GetModuleHandleA("api-ms-win-crt-runtime-l1-1-0.dll"), "_get_narrow_winmain_command_line");\
    Mhook_SetHook((LPVOID*)&OriginalGetWinmain, GetWinmainHook);\
    break;\
}\
case DLL_PROCESS_DETACH:\
{\
    break;\
}\
}\
\
return TRUE;\
}

#else

using TGetStartupInfoA = void(__stdcall*)(LPSTARTUPINFO lpStartupInfo);
extern TGetStartupInfoA OriginalGetStartupInfoA;

#define DEFINE_DLL_ENTRY_INITIALIZER(className) \
    BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)\
{\
switch (fdwReason)\
{\
case DLL_PROCESS_ATTACH:\
{\
    RegisterApp(std::make_unique<className>());\
    OriginalGetStartupInfoA = (GetStartupInfoA_t)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetStartupInfoA");\
    Mhook_SetHook((LPVOID*)& OriginalGetStartupInfoA, HookedGetStartupInfoA);\
    break;\
}\
case DLL_PROCESS_DETACH:\
{\
    break;\
}\
}\
\
return TRUE;\
}
#endif

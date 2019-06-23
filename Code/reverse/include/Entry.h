#pragma once

#include <Platform.h>

#if TP_PLATFORM_64

#define DEFINE_DLL_ENTRY_INITIALIZER \
    BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)\
{\
switch (fdwReason)\
{\
case DLL_PROCESS_ATTACH:\
{\
    OriginalGetWinmain = (_GetWinmain)GetProcAddress(GetModuleHandleA("api-ms-win-crt-runtime-l1-1-0.dll"), "_get_narrow_winmain_command_line");\
    Mhook_SetHook((LPVOID*)& OriginalGetWinmain, HookGetWinmain);\
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

#define DEFINE_DLL_ENTRY_INITIALIZER \
    BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)\
{\
switch (fdwReason)\
{\
case DLL_PROCESS_ATTACH:\
{\
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
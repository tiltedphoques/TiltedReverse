#include <windows.h>

#include <Entry.h>
#include <App.h>

#include <thread>
#include <mutex>

#include <mhook/mhook.h>

static std::unique_ptr<App> g_pApp;

using TWinMain = int(__stdcall)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
TWinMain* OriginalWinMain = nullptr;

static int __stdcall HookedWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Ensure exceptions won't cause our calls to be skipped
    struct ScopedCaller
    {
        ScopedCaller() { g_pApp->BeginMain(); }
        ~ScopedCaller() { g_pApp->EndMain(); }
    };

    ScopedCaller appCaller;

    return OriginalWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

static void SetupMainHook()
{
    OriginalWinMain = (TWinMain*)g_pApp->GetMainAddress();
    if (OriginalWinMain == nullptr)
        return;

    Mhook_SetHook((PVOID*)&OriginalWinMain, &HookedWinMain);
}

#if TP_PLATFORM_64

using TGetWinmain = char* (__stdcall*)();
TGetWinmain OriginalGetWinmain = nullptr;

char* GetWinmainHook()
{
    static std::once_flag s_flag;
    std::call_once(s_flag, SetupMainHook);

    return OriginalGetWinmain();
}

#else

using TGetStartupInfoA = void(__stdcall*)(LPSTARTUPINFO lpStartupInfo);
TGetStartupInfoA OriginalGetStartupInfoA = nullptr;

void __stdcall HookedGetStartupInfoA(LPSTARTUPINFO lpStartupInfo)
{
    static std::once_flag s_flag;
    std::call_once(s_flag, SetupMainHook);

    OriginalGetStartupInfoA(lpStartupInfo);
}

#endif

BOOL details::TiltedReverseMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved, std::function<std::unique_ptr<App>()> aAppFactory)
{
    TP_UNUSED(hModule);
    TP_UNUSED(lpReserved);

    switch (fdwReason)                                                                                                                                      
    {                                                                                                                                                       
    case DLL_PROCESS_ATTACH:                                                                                                                                
    {                                     
        g_pApp = aAppFactory();
#if TP_PLATFORM_64
        auto hmod = LoadLibraryA("api-ms-win-crt-runtime-l1-1-0.dll");
        if (hmod != 0)
        {
            OriginalGetWinmain = (TGetWinmain)GetProcAddress(hmod, "_get_narrow_winmain_command_line");
            Mhook_SetHook((LPVOID*)& OriginalGetWinmain, GetWinmainHook);
        }
#else
        auto hmod = LoadLibraryA("kernel32.dll");
        if (hmod != 0)
        {
            OriginalGetStartupInfoA = (TGetStartupInfoA)GetProcAddress(hmod, "GetStartupInfoA");
            Mhook_SetHook((LPVOID*)& OriginalGetStartupInfoA, HookedGetStartupInfoA);
        }
#endif

        g_pApp->Attach();

        break;                                                                                                                                              
    }                                                                                                                                                       
    case DLL_PROCESS_DETACH:                                                                                                                                
    {              
        g_pApp->Detach();

        break;                                                                                                                                              
    }                                                                                                                                                       
    }                                                                                                                                                       
        
    return TRUE;
}

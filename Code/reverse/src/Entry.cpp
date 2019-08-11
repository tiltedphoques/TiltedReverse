#include <windows.h>

#include <Entry.h>
#include <App.h>

#include <thread>
#include <mutex>

#include <mhook/mhook.h>
#include <Platform.h>
#include "FunctionHook.h"

static std::unique_ptr<App> g_pApp;

App& App::GetInstance()
{
    return *g_pApp;
}

using TWinMain = int(__stdcall)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
TWinMain* OriginalWinMain = nullptr;

static int __stdcall HookedWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Ensure exceptions won't cause our calls to be skipped
    struct ScopedCaller
    {
        ScopedCaller()
        {
            App::GetInstance().BeginMain();
            FunctionHookManager::GetInstance().InstallDelayedHooks();
        }
        ~ScopedCaller()
        {
            App::GetInstance().EndMain();
        }
    };

    ScopedCaller appCaller;

    return OriginalWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

static void SetupMainHook()
{
    OriginalWinMain = static_cast<TWinMain*>(App::GetInstance().GetMainAddress());
    if (OriginalWinMain == nullptr)
        return;

    Mhook_SetHook(reinterpret_cast<PVOID*>(&OriginalWinMain), &HookedWinMain);
}

#if TP_PLATFORM_64

using TGetWinmain = char* (__stdcall*)();
TGetWinmain OriginalGetWinmain = nullptr;

char* HookGetWinmain()
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

BOOL details::TiltedReverseMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved, const std::function<std::unique_ptr<App>()>& aAppFactory)
{
    TP_UNUSED(hModule);
    TP_UNUSED(lpReserved);

    switch (fdwReason)                                                                                                                                      
    {                                                                                                                                                       
    case DLL_PROCESS_ATTACH:                                                                                                                                
    {                                     
        g_pApp = aAppFactory();
#if TP_PLATFORM_64
        OriginalGetWinmain = reinterpret_cast<TGetWinmain>(TP_HOOK_SYSTEM("api-ms-win-crt-runtime-l1-1-0.dll", "_get_narrow_winmain_command_line", HookGetWinmain));
#else
        OriginalGetStartupInfoA = reinterpret_cast<TGetStartupInfoA>(TP_HOOK_SYSTEM("kernel32.dll", "GetStartupInfoA", HookedGetStartupInfoA));
#endif

        App::GetInstance().Attach();

        FunctionHookManager::GetInstance().InstallDelayedHooks();

        break;                                                                                                                                              
    }                                                                                                                                                       
    case DLL_PROCESS_DETACH:                                                                                                                                
    {              
        App::GetInstance().Detach();

        break;                                                                                                                                              
    }
    default: break;
    }                                                                                                                                                       
        
    return TRUE;
}

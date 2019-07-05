#include <windows.h>

#include <Entry.h>
#include <App.h>

#include <thread>
#include <mutex>

#include <mhook/mhook.h>

static std::unique_ptr<App> g_pApp;

void RegisterApp(std::unique_ptr<App> aApp)
{
    g_pApp = std::move(aApp);
}

using TWinMain = int(__stdcall)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
TWinMain* OriginalWinMain = nullptr;

static int __stdcall HookedWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    g_pApp->RegisterHooks();

    return OriginalWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

static void SetupMainHook()
{
    OriginalWinMain = (TWinMain*)g_pApp->GetMainAddress();

    Mhook_SetHook((PVOID*)&OriginalWinMain, &HookedWinMain);
}

#if TP_PLATFORM_64

TGetWinmain OriginalGetWinmain = nullptr;

void* GetWinmainHook()
{
    static std::once_flag s_flag;
    std::call_once(s_flag, SetupMainHook);

    return OriginalGetWinmain();
}

#else

TGetStartupInfoA OriginalGetStartupInfoA = nullptr;

void __stdcall HookedGetStartupInfoA(LPSTARTUPINFO lpStartupInfo)
{
    static std::once_flag s_flag;
    std::call_once(s_flag, SetupMainHook);

    OriginalGetStartupInfoA(lpStartupInfo);
}

#endif
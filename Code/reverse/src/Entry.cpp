#include <windows.h>

#include <Entry.h>
#include <App.h>

#include <thread>
#include <mutex>

#include <mhook/mhook.h>
#include <Platform.h>
#include "FunctionHook.h"

static std::unique_ptr<TiltedPhoques::App> g_pApp;

TiltedPhoques::App& TiltedPhoques::App::GetInstance() noexcept
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
	        TiltedPhoques::App::GetInstance().BeginMain();
	        TiltedPhoques::FunctionHookManager::GetInstance().InstallDelayedHooks();
        }
        ~ScopedCaller()
        {
	        TiltedPhoques::App::GetInstance().EndMain();
        }
    };

    ScopedCaller appCaller;

    return OriginalWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

static void SetupMainHook()
{
    OriginalWinMain = static_cast<TWinMain*>(TiltedPhoques::App::GetInstance().GetMainAddress());
    if (OriginalWinMain == nullptr)
        return;

    Mhook_SetHook(reinterpret_cast<PVOID*>(&OriginalWinMain), &HookedWinMain);
}

#if TP_PLATFORM_64

using TGetWinmain = char* (__stdcall*)();
using T__crtGetShowWindowMode = short(__stdcall*)();

TGetWinmain OriginalGetWinmain = nullptr;
T__crtGetShowWindowMode Original__crtGetShowWindowMode = nullptr;

static std::once_flag s_mainHookCallFlag;

char* __stdcall HookGetWinmain()
{
    std::call_once(s_mainHookCallFlag, SetupMainHook);

    return OriginalGetWinmain();
}

short __stdcall Hook__crtGetShowWindowMode()
{
    std::call_once(s_mainHookCallFlag, SetupMainHook);

    return Original__crtGetShowWindowMode();
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

namespace TiltedPhoques
{
	BOOL details::ReverseMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved, const std::function<std::unique_ptr<App>()>& aAppFactory) noexcept
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
			Original__crtGetShowWindowMode = reinterpret_cast<T__crtGetShowWindowMode>(TP_HOOK_SYSTEM("msvcr110.dll", "__crtGetShowWindowMode", Hook__crtGetShowWindowMode));
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
}
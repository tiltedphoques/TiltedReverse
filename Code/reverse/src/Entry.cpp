#include <windows.h>

#include <Entry.h>

#include <thread>
#include <mutex>

void SetupHooks()
{

}

#if TP_PLATFORM_64

TGetWinmain OriginalGetWinmain = nullptr;

void* GetWinmainHook()
{
    static std::once_flag s_flag;
    std::call_once(s_flag, SetupHooks);

    return OriginalGetWinmain();
}

#else

TGetStartupInfoA OriginalGetStartupInfoA = nullptr;

void __stdcall HookedGetStartupInfoA(LPSTARTUPINFO lpStartupInfo)
{
    static std::once_flag s_flag;
    std::call_once(s_flag, SetupHooks);

    OriginalGetStartupInfoA(lpStartupInfo);
}

#endif
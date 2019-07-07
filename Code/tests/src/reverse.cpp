#include "catch.hpp"

#include <windows.h>
#include <Platform.h>

int __stdcall FakeWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    CHECK(true);

    return 0;
}

TEST_CASE("Load the reverse dll", "[reverse]")
{
    SECTION("Load")
    {
#if TP_RELEASE
        auto handle = LoadLibraryA("DLL_r.dll");
#else
        auto handle = LoadLibraryA("DLL.dll");
#endif
        
        auto pSetParams = GetProcAddress(handle, "SetParams");

        using TSetParams = void(*)(void*);
        auto callableSetParams = (TSetParams)pSetParams;

        callableSetParams(&FakeWinMain);

#if TP_PLATFORM_64
        // This is a dirty trick to pretend like winmain hasn't been called yet as usually we would inject before the main function
        using TGetWinmain = void* (__stdcall*)();
        TGetWinmain OriginalGetWinmain = nullptr;

        OriginalGetWinmain = (TGetWinmain)GetProcAddress(LoadLibraryA("api-ms-win-crt-runtime-l1-1-0.dll"), "_get_narrow_winmain_command_line");

        CHECK(OriginalGetWinmain() != nullptr);
#endif

        FakeWinMain(0, 0, 0, 0);
    }
}
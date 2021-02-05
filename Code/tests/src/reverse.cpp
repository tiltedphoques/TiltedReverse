#include "catch.hpp"

#include <windows.h>
#include <AutoPtr.hpp>
#include <AutoPtrManager.hpp>
#include <TiltedCore/Platform.hpp>

using namespace TiltedPhoques;

int __stdcall FakeWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    CHECK(true);

    return 0;
}

TEST_CASE("Load the reverse dll", "[reverse.app]")
{
    SECTION("Load")
    {
#if TP_RELEASE
        const auto handle = LoadLibraryA("DLL_r.dll");
#else
        const auto handle = LoadLibraryA("DLL.dll");
#endif

        const auto pSetParams = GetProcAddress(handle, "SetParams");

        using TSetParams = void(*)(void*);
        const auto callableSetParams = reinterpret_cast<TSetParams>(pSetParams);

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

TEST_CASE("Reverse auto ptr", "[reverse.autoptr]")
{
    SECTION("Use auto ptr manager")
    {
        REQUIRE(AutoPtrManager::GetInstance().GetBaseAddress() != 0);
    }

    SECTION("Simple init with address")
    {
        AutoPtr<int> x(0x12345678);
        int* ptr = x;
        REQUIRE(ptr == (int*)(0x12345678 + AutoPtrManager::GetInstance().GetBaseAddress()));
    }

    SECTION("Init from pattern")
    {
        AutoPtr<int> val(Pattern({ 0x78, 0x56, 0x34, 0x12 }, 1, Pattern::kDirect));
        REQUIRE(val.Get() != nullptr);
        REQUIRE(*val == 0x12345678);
    }
}

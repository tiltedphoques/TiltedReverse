#pragma once

namespace TiltedPhoques
{
    struct Debug
    {
        static void WaitForDebugger() noexcept;
        static void CreateConsole() noexcept;
        static void OnAttach() noexcept;
        static void OnDettach() noexcept;
    };
}

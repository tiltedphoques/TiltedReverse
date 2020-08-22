#include <Debug.hpp>
#include <thread>
#include <cassert>

#include <windows.h>

namespace TiltedPhoques
{
    PVOID vehHandler = NULL;

    void Debug::WaitForDebugger() noexcept
    {
        while (!IsDebuggerPresent())
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void Debug::CreateConsole() noexcept
    {
        if (AllocConsole())
        {
            FILE* file = freopen("CONOUT$", "w", stdout);
            assert(file != nullptr);
            SetConsoleTitleA("Tilted Reverse Console");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
        }
    }

    LONG WINAPI OnException(PEXCEPTION_POINTERS ExceptionInfo) {
        ReleaseCapture(); // Release Skyrim's mouse capture - best effort

        // Perhaps EXCEPTION_EXECUTE_HANDLER is more in order, in case someone after us handles the exception graceully?
        return EXCEPTION_CONTINUE_SEARCH;

        UNREFERENCED_PARAMETER(ExceptionInfo);
    }

    void Debug::OnAttach() noexcept
    {
        const int CALL_LAST = 0;
        const int CALL_FIRST = 1;
        assert(vehHandler == NULL);
        vehHandler = AddVectoredExceptionHandler(CALL_LAST, OnException);
        assert(vehHandler != NULL);
    }

    void Debug::OnDettach() noexcept
    {
        if (vehHandler) {
            RemoveVectoredExceptionHandler(vehHandler);
        }
    }
}

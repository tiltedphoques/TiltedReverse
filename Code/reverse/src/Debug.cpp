#include <Debug.h>
#include <thread>
#include <windows.h>

void Debug::WaitForDebugger()
{
    while (!IsDebuggerPresent())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

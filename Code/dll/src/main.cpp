#include <windows.h>

#include <Entry.hpp>
#include <App.hpp>

// See remark bellow
void* g_pMainAddress = nullptr;

struct TestApp : TiltedPhoques::App
{
    TestApp() {};
    virtual ~TestApp() {};

    void* GetMainAddress() const override
    {
        return g_pMainAddress;
    }

    void Update() override
    {

    }

    bool BeginMain() override
    {
        return true;
    }

    bool EndMain() override
    {
        return true;
    }

    bool Attach() override
    {
        return true;
    }

    bool Detach() override
    {
        return true;
    }
};


DEFINE_DLL_ENTRY_INITIALIZER(TestApp);

// Expose a function to give the address of the fake main function for test purposes, this makes no sense outside of a test framework so don't use this
extern "C"
{
    __declspec(dllexport) void SetParams(void* pMainFunctionAddress)
    {
        g_pMainAddress = pMainFunctionAddress;
    }
}

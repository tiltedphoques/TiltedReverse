#include <windows.h>

#include <Entry.h>
#include <App.h>

// See remark bellow
void* g_pMainAddress = nullptr;

struct TestApp : App
{
    TestApp() {};
    virtual ~TestApp() {};

    virtual void* GetMainAddress() const override
    {
        return g_pMainAddress;
    }

    virtual bool BeginMain() override
    {
        return true;
    }

    virtual bool EndMain() override
    {
        return true;
    }

    virtual bool Attach() override
    {
        return true;
    }

    virtual bool Detach() override
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
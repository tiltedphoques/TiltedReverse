#pragma once

#include <Stl.h>
#include <Windows.h>

struct FunctionHook
{
    FunctionHook();
    explicit FunctionHook(void** appSystemFunction, void* apHookFunction);
    ~FunctionHook();
    FunctionHook(const FunctionHook& acRhs) = delete;
    FunctionHook(FunctionHook&& aRhs) noexcept;
    FunctionHook& operator=(const FunctionHook& acRhs) = delete;
    FunctionHook& operator=(FunctionHook&& aRhs) noexcept;

protected:

    friend class FunctionHookManager;

    void** m_ppSystemFunction;
    void* m_pHookFunction;
};

class FunctionHookManager
{
public:

    FunctionHookManager(const FunctionHookManager&) = delete;
    FunctionHookManager(FunctionHookManager&&) = delete;

    FunctionHookManager& operator=(const FunctionHookManager&) = delete;
    FunctionHookManager& operator=(FunctionHookManager&&) = delete;

    void InstallDelayedHooks();
    void UninstallHooks();

    void Add(FunctionHook aFunctionHook, bool aDelayed = false);

    template<class T, class U>
    void Add(T** aSystemFunction, U* aHookFunction, bool aDelayed = false)
    {
        Add(FunctionHook(reinterpret_cast<void**>(aSystemFunction), reinterpret_cast<void*>(aHookFunction)), aDelayed);
    }

    template<class T>
    void* AddSystem(const std::string& acLibraryName, const std::string& acFunctionName, T* apFunction)
    {
        const auto module = LoadLibraryA(acLibraryName.c_str());
        if (module != nullptr)
        {
            auto pSystemFunction = reinterpret_cast<void*>(GetProcAddress(module, acFunctionName.c_str()));
            Add(&pSystemFunction, apFunction, false);

            return pSystemFunction;
        }

        return nullptr;
    }

    static FunctionHookManager& GetInstance()
    {
        static FunctionHookManager s_instance;
        return s_instance;
    }

private:

    FunctionHookManager();
    ~FunctionHookManager();

    Vector<FunctionHook> m_delayedHooks;
    Vector<FunctionHook> m_installedHooks;
};

#define TP_HOOK(systemFunction, hookFunction) FunctionHookManager::GetInstance().Add(systemFunction, hookFunction, true)
#define TP_HOOK_IMMEDIATE(systemFunction, hookFunction) FunctionHookManager::GetInstance().Add(systemFunction, hookFunction, false)

#define TP_HOOK_SYSTEM(libraryName, functionName, hookFunction) FunctionHookManager::GetInstance().AddSystem(libraryName, functionName, hookFunction)

#define TP_EMPTY_HOOK_PLACEHOLDER \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); \
__nop(); 
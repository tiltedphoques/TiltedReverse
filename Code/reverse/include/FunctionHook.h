#pragma once

#include <Stl.h>

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
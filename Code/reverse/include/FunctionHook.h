#pragma once

#include <Stl.h>
#include <Windows.h>

struct FunctionHook
{
    FunctionHook() noexcept;
    explicit FunctionHook(void** appSystemFunction, void* apHookFunction) noexcept;
    ~FunctionHook() noexcept;
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
protected:

    struct IATHook
    {
        void** pThunk;
        void* pOriginal;
    };

public:

    FunctionHookManager(const FunctionHookManager&) = delete;
    FunctionHookManager(FunctionHookManager&&) = delete;

    FunctionHookManager& operator=(const FunctionHookManager&) = delete;
    FunctionHookManager& operator=(FunctionHookManager&&) = delete;

    void InstallDelayedHooks() noexcept;
    void UninstallHooks() noexcept;

    void Add(FunctionHook aFunctionHook, bool aDelayed = false) noexcept;
    void* Add(void* apFunctionDetour, const char* acpLibraryName, const char* acpMethod) noexcept;

    template<class T, class U>
    void Add(T** aSystemFunction, U* aHookFunction, bool aDelayed = false) noexcept
    {
        Add(FunctionHook(reinterpret_cast<void**>(aSystemFunction), reinterpret_cast<void*>(aHookFunction)), aDelayed);
    }

    template<class T>
    void* AddSystem(const std::string& acLibraryName, const std::string& acFunctionName, T* apFunction) noexcept
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

    static FunctionHookManager& GetInstance() noexcept
    {
        static FunctionHookManager s_instance;
        return s_instance;
    }

private:

    FunctionHookManager() noexcept;
    ~FunctionHookManager() noexcept;

    Vector<FunctionHook> m_delayedHooks;
    Vector<FunctionHook> m_installedHooks;
    Vector<IATHook> m_iatHooks;
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
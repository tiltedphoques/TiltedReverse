#include <FunctionHook.h>
#include <windows.h>
#include <mhook.h>
#include <StackAllocator.h>


FunctionHook::FunctionHook()
    : m_ppSystemFunction(nullptr)
    , m_pHookFunction(nullptr)
{
}

FunctionHook::FunctionHook(void** appSystemFunction, void* apHookFunction)
    : m_ppSystemFunction(appSystemFunction)
    , m_pHookFunction(apHookFunction)
{
}

FunctionHook::~FunctionHook()
{
    if (m_ppSystemFunction != nullptr)
    {
        Mhook_Unhook(m_ppSystemFunction);
    }
}

FunctionHook::FunctionHook(FunctionHook&& aRhs) noexcept
    : FunctionHook()
{
    this->operator=(std::move(aRhs));
}

FunctionHook& FunctionHook::operator=(FunctionHook&& aRhs) noexcept
{
    std::swap(m_ppSystemFunction, aRhs.m_ppSystemFunction);
    std::swap(m_pHookFunction, aRhs.m_pHookFunction);

    return *this;
}

FunctionHookManager::FunctionHookManager() = default;

FunctionHookManager::~FunctionHookManager()
{
    UninstallHooks();
}

void FunctionHookManager::InstallDelayedHooks()
{
    StackAllocator<1 << 12> allocator;
    const auto pHooks = static_cast<HOOK_INFO*>(allocator.Allocate(sizeof(HOOK_INFO) * m_delayedHooks.size()));

    for (size_t i = 0; i < m_delayedHooks.size(); ++i)
    {
        pHooks[i].ppSystemFunction = m_delayedHooks[i].m_ppSystemFunction;
        pHooks[i].pHookFunction = m_delayedHooks[i].m_pHookFunction;
    }

    Mhook_SetHookEx(pHooks, m_delayedHooks.size());

    for (auto& hook : m_delayedHooks)
    {
        m_installedHooks.emplace_back(std::move(hook));
    }

    m_delayedHooks.clear();
}

void FunctionHookManager::UninstallHooks()
{
    StackAllocator<1 << 12> allocator;
    const auto pHooks = static_cast<void***>(allocator.Allocate(sizeof(void**) * m_installedHooks.size()));

    for (size_t i = 0; i < m_installedHooks.size(); ++i)
    {
        pHooks[i] = m_installedHooks[i].m_ppSystemFunction;
        m_installedHooks[i].m_ppSystemFunction = nullptr;
    }

    //Mhook_UnhookEx(pHooks, m_installedHooks.size());

    m_installedHooks.clear();
}

void FunctionHookManager::Add(FunctionHook aFunctionHook, const bool aDelayed)
{
    if (aDelayed)
        m_delayedHooks.emplace_back(std::move(aFunctionHook));
    else
    {
        if (Mhook_SetHook(aFunctionHook.m_ppSystemFunction, aFunctionHook.m_pHookFunction) == TRUE)
            m_installedHooks.emplace_back(std::move(aFunctionHook));
    }
}

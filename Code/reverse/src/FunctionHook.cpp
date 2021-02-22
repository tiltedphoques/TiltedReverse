#include <windows.h>
#include <MinHook.h>

#include <FunctionHook.hpp>
#include <TiltedCore/StackAllocator.hpp>
#include <ProcessMemory.hpp>

#define RtlOffsetToPointer(Base, Offset) ((PCHAR)(((PCHAR)(Base)) + ((ULONG_PTR)(Offset))))

namespace TiltedPhoques
{
    static void** GetImportedFunction(const wchar_t *acpModuleName, const char* acpLibraryName, const char* acpMethod) noexcept;

    FunctionHook::FunctionHook() noexcept
        : m_ppSystemFunction(nullptr)
        , m_pHookFunction(nullptr)
    {
    }

    FunctionHook::FunctionHook(void** appSystemFunction, void* apHookFunction) noexcept
        : m_ppSystemFunction(appSystemFunction)
        , m_pHookFunction(apHookFunction)
    {
    }

    FunctionHook::~FunctionHook() noexcept
    {
        if (m_ppSystemFunction != nullptr)
        {
            MH_DisableHook(*m_ppSystemFunction);
            MH_RemoveHook(*m_ppSystemFunction);
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

    FunctionHookManager::FunctionHookManager() noexcept
    {

    }

    FunctionHookManager::~FunctionHookManager() noexcept
    {
        UninstallHooks();
    }

    void FunctionHookManager::InstallDelayedHooks() noexcept
    {
        for (auto& hook : m_delayedHooks)
        {
            MH_CreateHook(*hook.m_ppSystemFunction, hook.m_pHookFunction, hook.m_ppSystemFunction);
            MH_EnableHook(*hook.m_ppSystemFunction);
            m_installedHooks.emplace_back(std::move(hook));
        }

        m_delayedHooks.clear();
    }

    void FunctionHookManager::UninstallHooks() noexcept
    {
        for (size_t i = 0; i < m_installedHooks.size(); ++i)
        {
            m_installedHooks[i].m_ppSystemFunction = nullptr;
        }

        //Mhook_UnhookEx(pHooks, m_installedHooks.size());

        m_installedHooks.clear();

        for (auto& iatHook : m_iatHooks)
        {
            const ProcessMemory thunkMemory(iatHook.pThunk, sizeof(iatHook.pThunk));
            thunkMemory.Write(iatHook.pOriginal);
        }
    }

    void FunctionHookManager::Add(FunctionHook aFunctionHook, const bool aDelayed) noexcept
    {
        if (aDelayed)
            m_delayedHooks.emplace_back(std::move(aFunctionHook));
        else if (MH_CreateHook(*aFunctionHook.m_ppSystemFunction, aFunctionHook.m_pHookFunction, aFunctionHook.m_ppSystemFunction) == MH_OK &&
                 MH_EnableHook(*aFunctionHook.m_ppSystemFunction) == MH_OK)
            m_installedHooks.emplace_back(std::move(aFunctionHook));
    }

    void* FunctionHookManager::Add(void* apFunctionDetour, const char* acpLibraryName, const char* acpMethod) noexcept
    {
        const auto pRealFunctionThunk = GetImportedFunction(nullptr, acpLibraryName, acpMethod);

        if (!pRealFunctionThunk)
            return nullptr;

        const auto pRealFunction = *pRealFunctionThunk;

        const ProcessMemory thunkMemory(pRealFunctionThunk, sizeof(void*));
        thunkMemory.Write(apFunctionDetour);

        m_iatHooks.emplace_back(IATHook{ pRealFunctionThunk, pRealFunction });

        return pRealFunction;
    }

    static void** GetImportedFunction(const wchar_t *acpModuleName, const char* acpLibraryName, const char* acpMethod) noexcept
    {
        const auto pBase = GetModuleHandleW(acpModuleName);

        const auto pImageDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pBase);
        auto pImageNtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(RtlOffsetToPointer(pBase, pImageDosHeader->e_lfanew));

        const auto pVirtualAddress = pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

        for (auto pImageImportDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(RtlOffsetToPointer(pBase, pVirtualAddress)); pImageImportDescriptor->Name; ++pImageImportDescriptor)
        {
            const auto pLibraryName = reinterpret_cast<const char*>RtlOffsetToPointer(pBase, pImageImportDescriptor->Name);

            if (!_stricmp(pLibraryName, acpLibraryName))
            {
                auto pImportAddressTable = reinterpret_cast<uintptr_t*>(RtlOffsetToPointer(pBase, pImageImportDescriptor->FirstThunk));
                auto pImageThunkData = reinterpret_cast<IMAGE_THUNK_DATA*>(RtlOffsetToPointer(pBase, pImageImportDescriptor->OriginalFirstThunk));

                while (const auto pOrdinal = pImageThunkData->u1.Ordinal)
                {
                    const auto pFunctionName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(RtlOffsetToPointer(pBase, pImageThunkData->u1.AddressOfData))->Name;

                    if (IMAGE_SNAP_BY_ORDINAL(pOrdinal))
                    {
                        // Skip ordinal functions, we just want named functions
                    }
                    else if (!_stricmp(pFunctionName, acpMethod))
                    {
                        return reinterpret_cast<void**>(pImportAddressTable);
                    }

                    ++pImageThunkData;
                    ++pImportAddressTable;
                }

                return nullptr;
            }
        }

        return nullptr;
    }
}

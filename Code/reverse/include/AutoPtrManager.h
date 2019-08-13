#pragma once

#include <Pattern.h>
#include <mutex>

struct AutoPtrManager
{
    AutoPtrManager(const AutoPtrManager&) = delete;
    AutoPtrManager(AutoPtrManager&&) = delete;
    AutoPtrManager& operator=(const AutoPtrManager&) = delete;
    AutoPtrManager& operator=(AutoPtrManager&&) = delete;

    [[nodiscard]] uintptr_t GetBaseAddress() const noexcept;
    [[nodiscard]] void* Find(Pattern aPattern) const noexcept;

    static AutoPtrManager& GetInstance() noexcept
    {
        return s_instance;
    }

private:

    AutoPtrManager() noexcept;
    ~AutoPtrManager() noexcept;

    uintptr_t m_baseAddress;
    uintptr_t m_textStartAddress;
    size_t m_textSize;
    uint64_t m_textHash;

    static AutoPtrManager s_instance;
};
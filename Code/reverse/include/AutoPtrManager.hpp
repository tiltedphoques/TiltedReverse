#pragma once

#include <Pattern.hpp>

namespace TiltedPhoques
{
    struct AutoPtrManager
    {
        TP_NOCOPYMOVE(AutoPtrManager);

        struct Register
        {
            Register(uint16_t aId, Pattern aPattern);
        };

        static const uint16_t kMaxId = 1023;

        [[nodiscard]] uintptr_t GetBaseAddress() const noexcept;
        [[nodiscard]] uintptr_t Find(const Pattern& aPattern) const noexcept;
        [[nodiscard]] bool Add(uint16_t aId, const Pattern& acPattern) noexcept;
        void Commit() noexcept;

        // Some inlining please
        [[nodiscard]] static AutoPtrManager& GetInstance() noexcept
        {
            return s_instance;
        }

        [[nodiscard]] uintptr_t GetById(uint16_t aId) const noexcept
        {
            return m_pointers[aId];
        }

    private:

        AutoPtrManager() noexcept;
        ~AutoPtrManager() noexcept = default;

        void LoadFromDisk() noexcept;
        void SaveToDisk() noexcept;

        uintptr_t m_baseAddress;
        uintptr_t m_textStartAddress;
        size_t m_textSize;
        uint64_t m_textHash;
        std::array<uintptr_t, kMaxId + 1> m_pointers{};
        uint16_t m_pointerCount{0};

        static AutoPtrManager s_instance;
    };
}

#define RegisterPointer(name, ...) static TiltedPhoques::AutoPtrManager::Register name##ptrRegister(k ## name, __VA_ARGS__);

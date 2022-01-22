#pragma once

#include <Windows.h>
#include <AutoPtr.hpp>

#include <mem/mem.h>
#include <mem/module.h>
#include <mem/protect.h>

namespace TiltedPhoques::vp
{
    class ScopedContext
    {
    public:
        inline ScopedContext(const mem::pointer aEa, size_t aSize) noexcept;
        inline ~ScopedContext();

        template<class T>
        bool Write(const T& acData, const size_t aOffset = 0) const noexcept
        {
            return WriteBuffer(reinterpret_cast<const unsigned char*>(&acData), sizeof(T), aOffset);
        }

        inline bool WriteBuffer(const uint8_t* acpData, size_t aSize, size_t aOffset) const noexcept;

    private:
        size_t m_size;
        uint32_t m_oldProtect;
        mem::pointer m_ea;
    };

    MEM_STRONG_INLINE void Nop(const mem::pointer acAddress, size_t aLength) noexcept
    {
        const ScopedContext ctx(acAddress, aLength);
        std::memset(acAddress.as<void*>(), 0x90, aLength);
    }

    template<typename T>
    MEM_STRONG_INLINE bool Put(const mem::pointer aEa, const T acVal, const size_t acOffset = 0) noexcept
    {
        // https://github.com/0x1F9F1/Open1560/blob/ff7a5695f038867605345d933cb3e2cd6e9623b0/code/midtown/core/hooking.cpp
        mem::protect({ aEa, sizeof(T) }).copy(acVal);
    }
}

#if defined(_WIN32)
#include <Memory_Win.inl>
#endif
// TODO: maybe Linux?

#pragma once

#include <Windows.h>
#include <AutoPtr.hpp>

namespace TiltedPhoques
{
    namespace detail
    {
        // dumbs down a class member ptr to a generic
        // void ptr, this is technically undefined behavior
        // but is verified to always work on MSVC
        // https://stackoverflow.com/questions/8121320/get-memory-address-of-member-function
        template<typename TOut, typename TIn>
        TOut PunType(TIn in)
        {
            union
            {
                TIn  in;
                TOut out;
            }
            u = { in };

            return u.out;
        };

        class ProcessMemory
        {
        public:
            inline ProcessMemory(void* apMemoryLocation, size_t aSize) noexcept;
            inline ~ProcessMemory();

            template<class T>
            bool Write(const T& acData, const size_t aOffset = 0) const noexcept
            {
                return WriteBuffer(reinterpret_cast<const unsigned char*>(&acData), sizeof(T), aOffset);
            }

            inline bool WriteBuffer(const unsigned char* acpData, size_t aSize, size_t aOffset) const noexcept;

        private:

            unsigned long m_oldProtect;
            void* m_pMemoryLocation;
            size_t m_size;
        };
    }

    template<typename TVar, typename TAddress>
    bool Write(const TAddress acAddress, const TVar acVal, const size_t acOffset = 0) noexcept
    {
        AutoPtr<TVar> ptr(static_cast<uintptr_t>(acAddress));

        const detail::ProcessMemory memory(ptr.Get(), sizeof(TVar));
        return memory.Write(acVal, acOffset);
    }

    template<typename TAddress>
    bool Nop(const TAddress acAddress, size_t aLength) noexcept
    {
        AutoPtr<TAddress> ptr(static_cast<uintptr_t>(acAddress));

        const detail::ProcessMemory memory(ptr.Get(), aLength);
        return memory.WriteBuffer(reinterpret_cast<const unsigned char*>(ptr.Get()), aLength, 0);
    }

    // jump function
    template<typename TNewFunc, typename TAddr>
    void Jump(TAddr aAdress, TNewFunc aNew) noexcept
    {
        Write<uint8_t>(aAdress, 0xE9);
        Write<int32_t>((uintptr_t)aAdress + 1, (intptr_t)aNew - (intptr_t)aAdress - 5);
    }

    // jump class member function
    template<typename TNewFunc>
    void JumpCMF(uintptr_t aOldAddress, const TNewFunc &aNew) noexcept
    {
        const void* pNewFunc = detail::PunType<const void*>(aNew);
        Jump(aOldAddress, pNewFunc);
    }
}

#if defined(_WIN32)
#include <Memory_Win.inl>
#endif
// TODO: maybe Linux?

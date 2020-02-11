#pragma once

#include <AutoPtr.hpp>
#include <type_traits>

namespace TiltedPhoques
{
    struct ProcessMemory
    {
        ProcessMemory(void* apMemoryLocation, size_t aSize) noexcept;
        ~ProcessMemory();

        template<class T, typename = std::enable_if_t<std::is_pod<T>::value>>
        bool Write(const T & acData, const size_t aOffset = 0) const noexcept
        {
            return WriteBuffer(reinterpret_cast<const unsigned char*>(&acData), sizeof(T), aOffset);
        }

        bool WriteBuffer(const unsigned char* acpData, size_t aSize, size_t aOffset) const noexcept;

    private:

        unsigned long m_oldProtect;
        void* m_pMemoryLocation;
        size_t m_size;
    };

    template<typename TVar, typename TAddr>
    inline bool Write(const TAddr addr, const TVar val, const size_t offset = 0) {
        AutoPtr<TVar> ptr((uintptr_t)(addr));

        ProcessMemory mem(ptr.GetPtr(), sizeof(TVar));
        return mem.Write(val, offset);
    }

    template<typename TAddr>
    inline bool Nop(const TAddr addr, size_t len) {
        AutoPtr<TAddr> ptr((uintptr_t)addr);

        ProcessMemory mem(ptr.GetPtr(), len);
        return mem.WriteBuffer(reinterpret_cast<const unsigned char*>(ptr.GetPtr()), len, 0);
    }
}

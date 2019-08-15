#pragma once

#include <type_traits>

struct ProcessMemory
{
    ProcessMemory(void* apMemoryLocation, size_t aSize) noexcept;
    ~ProcessMemory();

    template<class T, typename = std::enable_if_t<std::is_pod<T>::value>>
    bool Write(const T& acData, const size_t aOffset = 0) const noexcept
    {
        return WriteBuffer(reinterpret_cast<const unsigned char*>(acData), sizeof(T), aOffset);
    }

    bool WriteBuffer(const unsigned char* acpData, size_t aSize, size_t aOffset) const noexcept;

private:

    unsigned long m_oldProtect;
    void* m_pMemoryLocation;
    size_t m_size;
};

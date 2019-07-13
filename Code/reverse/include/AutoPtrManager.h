#pragma once

#include <Stl.h>
#include <Pattern.h>
#include <mutex>

struct AutoPtrManager
{
    AutoPtrManager();
    ~AutoPtrManager();

    uintptr_t GetBaseAddress() const;
    void* Find(Pattern aPattern);

    static AutoPtrManager& GetInstance()
    {
        return s_instance;
    }

private:

    uintptr_t m_baseAddress;
    uintptr_t m_textStartAddress;
    size_t m_textSize;
    uint64_t m_textHash;

    static AutoPtrManager s_instance;
};
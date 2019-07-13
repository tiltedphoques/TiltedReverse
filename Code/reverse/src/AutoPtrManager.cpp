#pragma warning(disable: 4073)
#pragma init_seg(lib)

#include <AutoPtrManager.h>
#include <windows.h>
#include <Hash.h>
#include <algorithm>
#include <assert.h>

AutoPtrManager AutoPtrManager::s_instance;

AutoPtrManager::AutoPtrManager()
    : m_baseAddress(0)
{
    m_baseAddress = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

    auto NTHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(m_baseAddress + static_cast<uintptr_t>(reinterpret_cast<PIMAGE_DOS_HEADER>(m_baseAddress)->e_lfanew));

    auto pSection = reinterpret_cast<const IMAGE_SECTION_HEADER*>(NTHeaders + 1);
    for (auto i = 0; i < NTHeaders->FileHeader.NumberOfSections; ++i, ++pSection)
    {
        if (strcmp((const char*)pSection->Name, ".text") == 0)
        {
            m_textStartAddress = m_baseAddress + pSection->VirtualAddress;
            m_textSize = pSection->SizeOfRawData;
            break;
        }
    }

    m_textHash = FHash::Crc64(reinterpret_cast<const unsigned char*>(m_textStartAddress), m_textSize);
}

AutoPtrManager::~AutoPtrManager()
{

}

uintptr_t AutoPtrManager::GetBaseAddress() const
{
    return m_baseAddress;
}

void* AutoPtrManager::Find(Pattern aPattern)
{
    const uint8_t* cstart = (const uint8_t*)m_textStartAddress;
    const uint8_t* cend = cstart + m_textSize;

    auto cmpOp = [](uint8_t a, uint8_t b)
    {
        return (a == b || b == 0xCC);
    };

    size_t i = 0;
    while (true)
    {
        auto res = std::search(cstart, cend, aPattern.BytePattern.begin(), aPattern.BytePattern.end(), cmpOp);
        if (res >= cend)
            break;

        if (aPattern.Index == i)
        {
            if (aPattern.Type == Pattern::RelativeIndirection4)
            {
                auto address = res + aPattern.Offset;
                return (void*)(address + *reinterpret_cast<const int32_t*>(address) + 4);
            }
            return (void*)(res + aPattern.Offset);
            
        }

        ++i;

        cstart = res + aPattern.BytePattern.size();
    }

    return nullptr;
}

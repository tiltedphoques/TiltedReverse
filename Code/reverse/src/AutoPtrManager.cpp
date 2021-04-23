#pragma warning(disable: 4073)
#pragma init_seg(lib)

#include <AutoPtrManager.hpp>
#include <TiltedCore/Hash.hpp>

#include <Windows.h>

#include <cassert>
#include <filesystem>
#include <fstream>

#include <mem/module.h>
#include <mem/pattern.h>

namespace TiltedPhoques
{
    AutoPtrManager AutoPtrManager::s_instance;

    AutoPtrManager::AutoPtrManager() noexcept
        : m_baseAddress(0)
        , m_textStartAddress(0)
        , m_textSize(0)
    {
        mem::module mainModule = mem::module::main();
        auto* pImage = GetModuleHandle(nullptr);
        auto sectionHeaders = mainModule.section_headers();
        m_baseAddress = reinterpret_cast<uintptr_t>(pImage);

        for (const auto& cHeader : sectionHeaders)
        {
            if (memcmp(cHeader.Name, ".text", 5) == 0)
            {
                mem::region textRegion = mem::region(reinterpret_cast<uint8_t*>(m_baseAddress + cHeader.VirtualAddress),
                    cHeader.Misc.VirtualSize);

                m_textStartAddress = textRegion.start.as<uintptr_t>();
                m_textSize = textRegion.size;

                break;
            }
        }

        m_textHash = FHash::Crc64(reinterpret_cast<const unsigned char*>(m_textStartAddress), m_textSize);

        LoadFromDisk();
    }

    void AutoPtrManager::LoadFromDisk() noexcept
    {
        std::error_code ec;
        std::filesystem::create_directory("transient", ec);
        if (!ec)
        {
            std::filesystem::path p("transient");
            p /= std::to_string(m_textHash) + ".tpp";
            std::ifstream file(p.string().c_str(), std::ios::binary);
            if (file)
            {
                uint16_t count = 0;
                file.read(reinterpret_cast<char*>(&count), 2);
                file.read(reinterpret_cast<char*>(m_pointers.data()), static_cast<size_t>(count) * sizeof(uintptr_t));
                for (uint16_t i = 0; i < count; ++i)
                    m_pointers[i] += GetBaseAddress();
            }
        }
    }

    void AutoPtrManager::SaveToDisk() noexcept
    {
        std::error_code ec;
        std::filesystem::create_directory("transient", ec);
        if (!ec)
        {
            std::filesystem::path p("transient");
            p /= std::to_string(m_textHash) + ".tpp";
            std::ofstream file(p.string().c_str(), std::ios::binary | std::ios::trunc);
            if (file)
            {
                file.write(reinterpret_cast<char*>(&m_pointerCount), 2);

                decltype(m_pointers) savePtrs;
                for (uint16_t i = 0; i < m_pointerCount; ++i)
                    savePtrs[i] = m_pointers[i] - GetBaseAddress();

                file.write(reinterpret_cast<char*>(savePtrs.data()), static_cast<size_t>(m_pointerCount) * sizeof(uintptr_t));
            }
        }
    }

    AutoPtrManager::Register::Register(uint16_t aId, Pattern aPattern)
    {
        auto result = GetInstance().Add(aId, aPattern);
        assert(result);
    }

    uintptr_t AutoPtrManager::GetBaseAddress() const noexcept
    {
        return m_baseAddress;
    }

    uintptr_t AutoPtrManager::Find(const Pattern& aPattern) const noexcept
    {
        size_t i = 0;

        const mem::region cTextRegion(m_textStartAddress, m_textSize);
        const mem::pattern cPattern(aPattern.BytePattern);
        const mem::default_scanner cScanner(cPattern);

        auto results = cScanner.scan_all(cTextRegion);

        if (results.size() != aPattern.Expected && aPattern.Expected != 0) [[unlikely]]
        {
            return 0;
        }

        auto* const cpAddress = results[aPattern.Index].as<uint8_t*>();

        if (aPattern.Type == Pattern::kRelativeIndirection4)
        {
            auto* const address = cpAddress + aPattern.Offset;
            return reinterpret_cast<uintptr_t>(address + *reinterpret_cast<const int32_t*>(address) + 4);
        }

        return reinterpret_cast<uintptr_t>(cpAddress + aPattern.Offset);
    }

    bool AutoPtrManager::Add(uint16_t aId, const Pattern& acPattern) noexcept
    {
        assert(aId < 1024);

        m_pointerCount = std::max(static_cast<uint16_t>(aId + 1), m_pointerCount);
        if (m_pointers[aId] != 0)
            return true;

        const auto ptr = Find(acPattern);
        if (ptr == 0)
            return false;

        m_pointers[aId] = ptr;

        return true;
    }

    void AutoPtrManager::Commit() noexcept
    {
        SaveToDisk();
    }
}

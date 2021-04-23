#pragma once

#include <TiltedCore/Stl.hpp>

namespace TiltedPhoques
{
    struct Pattern
    {
        enum EType
        {
            kRelativeIndirection4,
            kDirect
        };

        Pattern(const char* acpBytePattern, size_t aExpected = 1, EType aPatternType = kDirect, intptr_t aOffset = 0, size_t aIndex = 0) noexcept;

        const char* BytePattern;
        size_t Expected;
        EType Type;
        intptr_t Offset{ 0 };
        size_t Index{ 0 };
    };
}

namespace std
{
    template<> struct less<TiltedPhoques::Pattern>
    {
        bool operator() (const TiltedPhoques::Pattern& lhs, const TiltedPhoques::Pattern& rhs) const noexcept
        {
            return lhs.BytePattern < rhs.BytePattern;
        }
    };
}

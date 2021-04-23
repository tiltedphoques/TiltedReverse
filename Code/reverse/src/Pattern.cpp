#include <Pattern.hpp>

namespace TiltedPhoques
{
    Pattern::Pattern(const char* acpBytePattern, const size_t aExpected, const EType aPatternType, const intptr_t aOffset /* = 0 */, const size_t aIndex /* = 0 */) noexcept
        : BytePattern(acpBytePattern)
        , Expected(aExpected)
        , Type(aPatternType)
        , Offset(aOffset)
        , Index(aIndex)
    {
    }
}

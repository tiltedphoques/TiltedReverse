#include <Pattern.h>

Pattern::Pattern(Vector<uint8_t> aBytePattern, size_t aExpected, Pattern::EType aPatternType, intptr_t aOffset /* = 0 */, size_t aIndex /* = 0 */)
    : BytePattern(std::move(aBytePattern))
    , Expected(aExpected)
    , Type(aPatternType)
    , Offset(aOffset)
    , Index(aIndex)
{
}
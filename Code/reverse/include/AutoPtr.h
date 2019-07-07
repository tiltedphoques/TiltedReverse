#pragma once

#include <cstdint>

struct AutoPtr
{
public:

#if TP_PLATFORM_64
    AutoPtr() = default;
#else
    AutoPtr() = delete;
#endif

    AutoPtr(AutoPtr&) = delete;
    AutoPtr& operator=(AutoPtr&) = delete;
};
#pragma once

#include <Platform.h>
#include <Pattern.h>

struct BasicAutoPtr
{
    BasicAutoPtr(Pattern aPattern);
    BasicAutoPtr(uintptr_t aAddress);

    BasicAutoPtr() = delete;
    BasicAutoPtr(BasicAutoPtr&) = delete;
    BasicAutoPtr& operator=(BasicAutoPtr&) = delete;

    void* GetPtr();

private:

    void* m_pPtr;
};

template<class T>
struct AutoPtr : BasicAutoPtr
{
    AutoPtr(Pattern aPattren) : BasicAutoPtr(std::move(aPattren)) {}
    AutoPtr(uintptr_t aAddress) : BasicAutoPtr(aAddress) {}

    AutoPtr() = delete;
    AutoPtr(AutoPtr&) = delete;
    AutoPtr& operator=(AutoPtr&) = delete;

    operator T* () { return Get(); }
    T* operator->() { return Get(); }

    T* Get() { return (T*)GetPtr(); }
};
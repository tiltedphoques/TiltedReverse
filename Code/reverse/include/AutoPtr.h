#pragma once

#include <Pattern.h>

struct BasicAutoPtr
{
    explicit BasicAutoPtr(Pattern aPattern);
    explicit BasicAutoPtr(uintptr_t aAddress);

    BasicAutoPtr() = delete;
    BasicAutoPtr(BasicAutoPtr&) = delete;
    BasicAutoPtr& operator=(BasicAutoPtr&) = delete;

    [[nodiscard]] void* GetPtr() const;

private:

    void* m_pPtr;
};

template<class T>
struct AutoPtr : BasicAutoPtr
{
    explicit AutoPtr(Pattern aPattren) : BasicAutoPtr(std::move(aPattren)) {}
    explicit AutoPtr(const uintptr_t aAddress) : BasicAutoPtr(aAddress) {}

    AutoPtr() = delete;
    AutoPtr(AutoPtr&) = delete;
    AutoPtr& operator=(AutoPtr&) = delete;

    explicit operator T* () { return Get(); }
    T* operator->() { return Get(); }

    T* Get() { return static_cast<T*>(GetPtr()); }
};
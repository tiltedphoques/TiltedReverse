#pragma once

#include <Pattern.h>

struct BasicAutoPtr
{
    explicit BasicAutoPtr(Pattern aPattern) noexcept;
    explicit BasicAutoPtr(uintptr_t aAddress) noexcept;

    BasicAutoPtr() = delete;
    BasicAutoPtr(BasicAutoPtr&) = delete;
    BasicAutoPtr& operator=(BasicAutoPtr&) = delete;

    [[nodiscard]] void* GetPtr() const noexcept;

private:

    void* m_pPtr;
};

template<class T>
struct AutoPtr : BasicAutoPtr
{
    explicit AutoPtr(Pattern aPattren) noexcept : BasicAutoPtr(std::move(aPattren)) {}
    explicit AutoPtr(const uintptr_t aAddress) noexcept : BasicAutoPtr(aAddress) {}

    AutoPtr() = delete;
    AutoPtr(AutoPtr&) = delete;
    AutoPtr& operator=(AutoPtr&) = delete;

    explicit operator T* () noexcept { return Get(); }
    T* operator->() noexcept { return Get(); }

    T* Get() noexcept { return static_cast<T*>(GetPtr()); }
};

template<class TFunc, class TThis, class... TArgs>
constexpr auto ThisCall(AutoPtr<TFunc>& aFunction, AutoPtr<TThis>& aThis, TArgs&& ... args) noexcept
{
#if TP_PLATFORM_64
    return aFunction.Get()(aThis.Get(), std::forward<TArgs>(args)...);
#else
    return aFunction.Get()(aThis.Get(), nullptr, std::forward<TArgs>(args)...);
#endif
}
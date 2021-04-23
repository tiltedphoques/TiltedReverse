#pragma once

#include <AutoPtrManager.hpp>

namespace TiltedPhoques
{
    template<class T>
    struct AutoPtr
    {
        explicit AutoPtr(uint16_t aId) noexcept;
        ~AutoPtr() = default;

        AutoPtr() = delete;
        AutoPtr(const AutoPtr&) = default;
        AutoPtr(AutoPtr&&) = default;
        AutoPtr& operator=(const AutoPtr&) = default;
        AutoPtr& operator=(AutoPtr&&) = default;

        operator T* () const noexcept { return Get(); }
        T* operator->() const noexcept { return Get(); }
        T* Get() const noexcept;

    private:

        uint16_t m_id;
    };

    template <class T>
    AutoPtr<T>::AutoPtr(uint16_t aId) noexcept
        : m_id(aId)
    {
        assert(aId <= AutoPtrManager::kMaxId);
    }

    template <class T>
    T* AutoPtr<T>::Get() const noexcept
    {
        return reinterpret_cast<T*>(AutoPtrManager::GetInstance().GetById(m_id));
    }
}

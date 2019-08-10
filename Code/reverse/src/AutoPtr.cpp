#include <AutoPtr.h>
#include <AutoPtrManager.h>

#include <cassert>

BasicAutoPtr::BasicAutoPtr(Pattern aPattern)
{
    // This fails if we aren't done unpacking
    //assert(App::GetInstance().IsReady());

    m_pPtr = AutoPtrManager::GetInstance().Find(std::move(aPattern));

    assert(m_pPtr != nullptr);
}

BasicAutoPtr::BasicAutoPtr(uintptr_t aAddress)
{
    m_pPtr = reinterpret_cast<void*>(aAddress + AutoPtrManager::GetInstance().GetBaseAddress());
}

void* BasicAutoPtr::GetPtr() const
{
    return m_pPtr;
}

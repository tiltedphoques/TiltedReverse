#include <AutoPtr.h>
#include <AutoPtrManager.h>

#include <App.h>

#include <assert.h>

BasicAutoPtr::BasicAutoPtr(Pattern aPattern)
{
    // This fails if we aren't done unpacking
    //assert(App::GetInstance().IsReady());

    m_pPtr = AutoPtrManager::GetInstance().Find(std::move(aPattern));

    assert(m_pPtr != nullptr);
}

BasicAutoPtr::BasicAutoPtr(uintptr_t aAddress)
{
    m_pPtr = (void*)(aAddress + AutoPtrManager::GetInstance().GetBaseAddress());
}

void* BasicAutoPtr::GetPtr()
{
    return m_pPtr;
}
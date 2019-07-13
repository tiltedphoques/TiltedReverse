#pragma once

#include <list>
#include <memory>
#include <Stl.h>

// These will run 
struct FunctionHook
{
    FunctionHook();
    virtual ~FunctionHook();

    virtual void Install() = 0;
    virtual void Uninstall() = 0;
};

class FunctionHookManager
{
public:

    FunctionHookManager();
    ~FunctionHookManager();

    void Add(std::unique_ptr<FunctionHook> aFunctionHook);

    static FunctionHookManager& GetInstance()
    {
        static FunctionHookManager s_instance;
        return s_instance;
    }

private:

    List<std::unique_ptr<FunctionHook>> m_functionHooks;
};

template<class T>
struct FunctionHookInstaller
{
    FunctionHookInstaller()
    {
        FunctionHookManager::GetInstance().Add(std::make_unique<T>());
    }
};

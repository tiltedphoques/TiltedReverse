#include <FunctionHook.h>


FunctionHook::FunctionHook()
{
}

FunctionHook::~FunctionHook()
{

}

FunctionHookManager::FunctionHookManager()
{

}

FunctionHookManager::~FunctionHookManager()
{

}

void FunctionHookManager::Add(std::unique_ptr<FunctionHook> aFunctionHook)
{
    m_functionHooks.emplace_back(std::move(aFunctionHook));
}
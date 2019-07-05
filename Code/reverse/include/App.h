#pragma once

struct App
{
    App();
    virtual ~App();

    virtual void* GetMainAddress() const = 0;
    virtual bool RegisterHooks() = 0;
};
#pragma once

struct Initializer
{
    Initializer();
    virtual ~Initializer();

    virtual bool Init() = 0;
    virtual bool Destroy() = 0;
};
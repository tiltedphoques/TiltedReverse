#include <App.h>

App::App()
    : m_ready(false)
{
}

App::~App()
{
}

void App::Start()
{
    m_ready = true;

    BeginMain();
}

bool App::IsReady() const
{
    return m_ready;
}
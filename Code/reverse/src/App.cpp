#include <App.h>

App::App() noexcept
    : m_ready(false)
{
}

App::~App() = default;

void App::Start() noexcept
{
    m_ready = true;

    BeginMain();
}

bool App::IsReady() const noexcept
{
    return m_ready;
}
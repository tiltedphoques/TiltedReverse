#include <App.hpp>
#include <Memory.hpp>
#include <TiltedCore/Initializer.hpp>

namespace TiltedPhoques
{
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
}

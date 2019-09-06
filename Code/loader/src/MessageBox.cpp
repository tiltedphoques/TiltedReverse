#include <windows.h>
#include <MessageBox.hpp>

#include <Utils.hpp>

MessageBox::MessageBox()
    : m_type(MB_OK)
{
}

void MessageBox::SetTitleSuffix(const std::wstring& aSuffix)
{
    m_titleSuffix = aSuffix;
}

void MessageBox::ModType(uint32_t aType)
{
    m_type |= aType;
}

void MessageBox::Show() const
{
    std::wstring title = L"Tilted Phoques";
    if (!m_titleSuffix.empty())
    {
        title += L" " + m_titleSuffix;
    }

    MessageBoxW(nullptr, m_stream.str().c_str(), title.c_str(), m_type);
}

ErrorMessageBox::ErrorMessageBox()
    : ErrorMessageBox(GetLastError())
{
}

ErrorMessageBox::ErrorMessageBox(const uint32_t aErrorCode)
    : MessageBox()
{
    SetTitleSuffix(L"(code " + std::to_wstring(aErrorCode) + L")");
    ModType(MB_ICONERROR);

    *this << Utils::ErrorToString(aErrorCode) << L"\r\n";
}

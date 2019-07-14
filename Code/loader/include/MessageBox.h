#pragma once

#ifdef MessageBox
#undef MessageBox
#endif

#include <cstdint>
#include <string>
#include <sstream>

class MessageBox
{
public:

    MessageBox();

    ~MessageBox() = default;

    MessageBox(MessageBox&) = delete;
    MessageBox(MessageBox&&) = delete;

    MessageBox& operator=(const MessageBox&) = delete;
    MessageBox& operator=(const MessageBox&&) = delete;

    template<typename T>
    MessageBox& operator<<(T rhs)
    {
        m_stream << rhs;
        return *this;
    }

    void SetTitleSuffix(const std::wstring& aSuffix);
    void ModType(uint32_t aType);

    void Show();

private:

    uint32_t m_type;

    std::wstring m_titleSuffix;
    std::wostringstream m_stream;
};

class ErrorMessageBox : public MessageBox
{
public:

    ErrorMessageBox();
    ErrorMessageBox(uint32_t aErrorCode);
};

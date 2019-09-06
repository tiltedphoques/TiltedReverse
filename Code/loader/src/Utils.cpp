#include <windows.h>
#include <Utils.hpp>

std::wstring Utils::ErrorToString(const uint32_t aErrorCode)
{
    if (aErrorCode == 0)
    {
        return L"";
    }

    wchar_t* pBuffer = nullptr;
    const size_t size =
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, aErrorCode, 0, reinterpret_cast<LPWSTR>(&pBuffer), 0, nullptr);

    std::wstring message(pBuffer, size);
    LocalFree(pBuffer);

    return message;
}

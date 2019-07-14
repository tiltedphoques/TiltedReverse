#include <windows.h>
#include <Utils.h>

std::wstring Utils::ErrorToString(uint32_t errorCode)
{
    if (errorCode == 0)
    {
        return L"";
    }

    wchar_t* pBuffer = nullptr;
    size_t size =
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode, 0, reinterpret_cast<LPWSTR>(&pBuffer), 0, nullptr);

    std::wstring message(pBuffer, size);
    LocalFree(pBuffer);

    return message;
}

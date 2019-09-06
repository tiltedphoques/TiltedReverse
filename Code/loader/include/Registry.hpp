#pragma once

#include <MessageBox.hpp>
#include <Utils.hpp>

namespace Registry
{
    enum class Key : uint8_t
    {
        ClassesRoot,
        CurrentUser,
        LocalMachine,
        Users,
        CurrentConfig
    };

    template<typename T, typename... Args>
    std::enable_if_t<!std::conjunction_v<std::is_constructible<std::wstring, Args>...>, T> Read(Key, Args&&...)
    {
        static_assert(false, "Not all arguments are wide strings");
        return {};
    }

    template<typename T, typename... Args>
    std::enable_if_t<(std::is_integral_v<T> || std::is_same_v<T, std::wstring>) && std::conjunction_v<std::is_constructible<std::wstring, Args>...>, T>
    Read(const Key aKey, Args&&... aKeys)
    {
        T result = T();
        uint32_t resultSize;
        
        if constexpr (std::is_same_v<T, std::wstring>)
        {
            resultSize = 0;
        }
        else
        {
            resultSize = sizeof(result);
        }

        std::filesystem::path regPath;
        regPath = (regPath / ... / std::wstring(aKeys));

        HKEY key;
        std::wstring keyStr;

        switch (aKey)
        {
            case Key::ClassesRoot:
            {
                key = HKEY_CLASSES_ROOT;
                keyStr = L"HKEY_CLASSES_ROOT";

                break;
            }
            case Key::CurrentConfig:
            {
                key = HKEY_CURRENT_CONFIG;
                keyStr = L"HKEY_CURRENT_CONFIG";

                break;
            }
            case Key::LocalMachine:
            {
                key = HKEY_LOCAL_MACHINE;
                keyStr = L"HKEY_LOCAL_MACHINE";

                break;
            }
            case Key::Users:
            {
                key = HKEY_USERS;
                keyStr = L"HKEY_USERS";

                break;
            }
            default:
            {
                key = HKEY_CURRENT_USER;
                keyStr = L"HKEY_CURRENT_USER";

                break;
            }
        }

        auto subKey = regPath.parent_path();
        auto value = regPath.filename();

        auto showErrorAndTerminate = [&keyStr, &subKey, &value](uint32_t aReturnCode)
        {
            ErrorMessageBox messageBox(aReturnCode);
            messageBox << L"An error occured when trying to read " << value << L" from " << std::quoted(keyStr + std::filesystem::path::preferred_separator + subKey.wstring()) << L".";

            messageBox.Show();
            ExitProcess(EXIT_FAILURE);
        };

        if constexpr (std::is_same_v<T, std::wstring>)
        {
            auto returnCode = RegGetValue(key, subKey.c_str(), value.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, reinterpret_cast<LPDWORD>(&resultSize));
            if (returnCode != ERROR_SUCCESS)
            {
                showErrorAndTerminate(returnCode);
                return result;
            }

            auto size = resultSize / sizeof(wchar_t);

            // Exclude the NULL written by the Win32 API.
            size -= sizeof(wchar_t);
            result.resize(size);
        }

        uint32_t returnCode = 0;

        if constexpr (std::is_same_v<T, std::wstring>)
        {
            returnCode = RegGetValue(key, subKey.c_str(), value.c_str(), RRF_RT_REG_SZ, nullptr, &result[0], reinterpret_cast<LPDWORD>(&resultSize));
        }
        else
        {
            returnCode = RegGetValue(key, subKey.c_str(), value.c_str(), RRF_RT_REG_DWORD | RRF_RT_REG_QWORD, nullptr, &result, reinterpret_cast<LPDWORD>(&resultSize));
        }

        if (returnCode != ERROR_SUCCESS)
        {
            showErrorAndTerminate(returnCode);
        }

        return result;
    }
}

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <TiltedCore/Platform.hpp>

#ifdef PUBLIC_BUILD
constexpr auto s_dllKey = "DllRelease";
#else
constexpr auto s_dllKey = "DllDebug";
#endif

#if TP_PLATFORM_64
constexpr auto s_gameIdKey = "GameId64";
constexpr auto s_gameExeKey = "GamePath64";
#else
constexpr auto s_gameIdKey = "GameId32";
constexpr auto s_gameExeKey = "GamePath32";
#endif

struct Config
{
    Config(const std::string& acFileName);
    ~Config() = default;

    [[nodiscard]] const std::string& Get(const std::string& acKey) const;

private:

    std::unordered_map<std::string, std::string> m_keyValueMap;
};

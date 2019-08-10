#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

struct Config
{
    Config(const std::string& acFileName);
    ~Config() = default;

    [[nodiscard]] const std::string& Get(const std::string& acKey) const;

private:

    std::unordered_map<std::string, std::string> m_keyValueMap;
};
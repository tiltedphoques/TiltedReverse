#include <Config.h>

#include <fstream>

Config::Config(const std::string& acFileName)
{
    std::ifstream file(acFileName.c_str());
    if (file.is_open())
    {
        for (std::string line; std::getline(file, line); )
        {
            auto pos = line.find('=');
            auto key = line.substr(0, pos);
            auto value = line.substr(pos + 1);

            if (key.size() > 0 && value.size() > 0)
            {
                m_keyValueMap[key] = value;
            }
        }
    }
}


const std::string& Config::Get(const std::string& acKey) const
{
    static std::string s_result = "";

    auto it = m_keyValueMap.find(acKey);
    if (it != std::end(m_keyValueMap))
    {
        return it->second;
    }

    return s_result;
}
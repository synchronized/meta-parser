#pragma once

#include <string>

class GlobalConfig {
public:
    static GlobalConfig& Get();
    void Init(std::string module_name) {
        m_module_name = module_name;
    }

public:
    std::string m_module_name{};

private:
    GlobalConfig() = default;
    GlobalConfig(const GlobalConfig&) = delete;
    GlobalConfig(GlobalConfig&&) = delete;
    GlobalConfig& operator=(const GlobalConfig&) = delete;
    GlobalConfig& operator=(GlobalConfig&&) = delete;
};


inline GlobalConfig& GlobalConfig::Get() {
    static GlobalConfig inst;
    return inst;
}
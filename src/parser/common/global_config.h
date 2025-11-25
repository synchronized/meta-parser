#pragma once

#include <string>

class GlobalConfig {
public:
    static GlobalConfig& Get();
    void Init(std::string namespace_prefix) {
        m_namespace_prefix = namespace_prefix;
    }

public:
    std::string m_namespace_prefix{};
    std::string m_template_root{};
    std::string m_source_root{};
    std::string m_output_dir{};

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
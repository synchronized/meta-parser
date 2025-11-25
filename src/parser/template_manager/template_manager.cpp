
#include "template_manager.h"
#include "common/global_config.h"

std::string TemplateManager::loadTemplates(std::string template_name)
{
    auto it = m_template_pool.find(template_name);
    if (it != m_template_pool.end()) {
        return it->second;
    }
    std::string template_root = GlobalConfig::Get().m_template_root;
    std::string temp_path = template_root + "/" + template_name + ".mustache";
    std::string content;
    if (Utils::loadFile(temp_path, content)) {
        m_template_pool.insert_or_assign(template_name, content);
        return content;
    }
    return "";
}

bool TemplateManager::hasTemplate(std::string template_name) {
    return m_template_pool.end() != m_template_pool.find(template_name);
}

std::string TemplateManager::renderByTemplate(std::string template_name, Mustache::data& template_data)
{
    Mustache::mustache tmpl(loadTemplates(template_name));
    return tmpl.render(template_data);
}


#include "template_manager.h"

void TemplateManager::loadTemplates(std::string path, std::string template_name)
{
    std::string temp_path = path + "/" + template_name + ".mustache";
    std::string content;
    if (Utils::loadFile(temp_path, content)) {
        m_template_pool.insert_or_assign(template_name, content);
    }
}

bool TemplateManager::hasTemplate(std::string template_name) {
    return m_template_pool.end() != m_template_pool.find(template_name);
}

std::string TemplateManager::renderByTemplate(std::string template_name, Mustache::data& template_data)
{
    if (m_template_pool.end() == m_template_pool.find(template_name))
    {
        return "";
    }
    Mustache::mustache tmpl(m_template_pool[template_name]);
    return tmpl.render(template_data);
}

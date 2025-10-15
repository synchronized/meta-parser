#include "common/precompiled.h"

#include "generator/reflection_generator.h"

#include "language_types/class.h"
#include "template_manager/template_manager.h"

#include <map>
#include <set>

namespace Generator
{
    static const std::string single_cpp_temp_name = "reflection.gen.cpp";
    static const std::string single_h_temp_name = "reflection.gen.h";
    static const std::string all_temp_name = "reflection.all";

    ReflectionGenerator::ReflectionGenerator(
        std::string source_directory,
        std::string template_path,
        std::function<std::string(std::string)> get_include_function) 
            : GeneratorInterface(
                source_directory + "/_generated/reflection", 
                source_directory, 
                template_path,
                get_include_function)
    {
        prepareStatus(m_out_path);
    }
    void ReflectionGenerator::prepareStatus(std::string path)
    {
        GeneratorInterface::prepareStatus(path);
        auto tmpMgr = TemplateManager::getInstance();
        tmpMgr->loadTemplates(m_template_path, single_cpp_temp_name);
        tmpMgr->loadTemplates(m_template_path, single_h_temp_name);
        tmpMgr->loadTemplates(m_template_path, all_temp_name);
        return;
    }

    std::string ReflectionGenerator::processFileName(std::string path, std::string ext_name)
    {
        auto relativeDir = fs::path(path).filename().replace_extension("reflection.gen"+ext_name).string();
        return m_out_path + "/" + relativeDir;
    }

    int ReflectionGenerator::generate(std::string path, SchemaMoudle schema)
    {
        static const std::string vector_prefix = "std::vector<";

        std::string    headfile_path = processFileName(path, ".h");

        Mustache::data mustache_data;
        Mustache::data include_headfiles(Mustache::data::type::list);
        Mustache::data class_defines(Mustache::data::type::list);

        std::string rela_path = Utils::makeRelativePath(m_root_path, path);
        include_headfiles.push_back(Mustache::data("headfile_item", rela_path));

        std::map<std::string, bool> class_names;
        // class defs
        for (auto class_temp : schema.classes)
        {
            if (!class_temp->shouldCompile())
                continue;

            class_names.insert_or_assign(class_temp->m_name, false);
            class_names[class_temp->m_name] = true;

            std::map<std::string, std::pair<std::string, std::string>> vector_map;

            Mustache::data class_def;
            Mustache::data vector_defines(Mustache::data::type::list);

            genClassRenderData(class_temp, class_def);
            for (auto field : class_temp->m_fields)
            {
                if (!field->shouldCompile())
                    continue;
                bool is_array = field->isVector();
                if (is_array)
                {
                    std::string item_type = field->m_type_name;

                    item_type = Utils::getNameWithoutContainer(item_type);

                    vector_map[field->m_type_name] = std::make_pair(field->m_type_qualified_name, item_type);
                }
            }

            if (vector_map.size() > 0)
            {
                if (nullptr == class_def.get("vector_exist"))
                {
                    class_def.set("vector_exist", true);
                }
                for (auto vector_item : vector_map)
                {
                    std::string    array_useful_name = vector_item.second.first;
                    std::string    item_type         = vector_item.second.second;
                    Mustache::data vector_define;
                    vector_define.set("vector_useful_name", array_useful_name);
                    vector_define.set("vector_type_name", vector_item.first);
                    vector_define.set("vector_element_type_name", item_type);
                    vector_defines.push_back(vector_define);
                }
            }
            class_def.set("vector_defines", vector_defines);
            class_defines.push_back(class_def);
        }

        mustache_data.set("class_defines", class_defines);
        mustache_data.set("include_headfiles", include_headfiles);

        std::string tmp = Utils::convertNameToUpperCamelCase(fs::path(path).stem().string(), "_");
        //mustache_data.set("sourefile_name_upper_camel_case", tmp);

        auto tmpMgr = TemplateManager::getInstance();
        if (tmpMgr->hasTemplate(single_h_temp_name)) {
            std::string render_string = 
                tmpMgr->renderByTemplate(single_h_temp_name, mustache_data);
            Utils::saveFile(render_string, headfile_path);
        }
        if (tmpMgr->hasTemplate(single_cpp_temp_name)) {
            std::string sourcefile_path = processFileName(path, ".cpp");
            std::string render_string =
                tmpMgr->renderByTemplate(single_cpp_temp_name, mustache_data);
            Utils::saveFile(render_string, sourcefile_path);
        }

        std::string rela_file = Utils::makeRelativePath(m_root_path, headfile_path);

        mustache_data.set("generate_headfile_item", rela_file);
        m_mustache_data_list.push_back(mustache_data);
        return 0;
    }

    void ReflectionGenerator::finish()
    {
        auto tmpMgr = TemplateManager::getInstance();
        if (tmpMgr->hasTemplate(all_temp_name)) {
            Mustache::data mustache_data;

            mustache_data.set("sourefiles", m_mustache_data_list);

            std::string render_string =
                tmpMgr->renderByTemplate(all_temp_name, mustache_data);
            Utils::saveFile(render_string, m_out_path + "/reflection.all.h");
        }
    }

    ReflectionGenerator::~ReflectionGenerator() {}
} // namespace Generator
#include "common/precompiled.h"
#include "common/global_config.h"

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

    ReflectionGenerator::ReflectionGenerator(std::function<std::string(std::string)> get_include_function) 
            : GeneratorInterface(get_include_function)
    {
        prepareStatus();
    }
    void ReflectionGenerator::prepareStatus()
    {
        auto tmpMgr = TemplateManager::getInstance();
        tmpMgr->loadTemplates(single_cpp_temp_name);
        tmpMgr->loadTemplates(single_h_temp_name);
        tmpMgr->loadTemplates(all_temp_name);
        return;
    }

    std::string ReflectionGenerator::processFileName(std::string path, std::string ext_name)
    {
        auto relativeDir = fs::path(path).filename().replace_extension("reflection.gen"+ext_name).string();
        auto output_dir = GlobalConfig::Get().m_output_dir;
        return output_dir + "/" + relativeDir;
    }

    int ReflectionGenerator::generate(std::string path, SchemaMoudle schema)
    {

        std::string    headfile_path = processFileName(path, ".h");

        Mustache::data mustache_data;
        Mustache::data include_headfiles(Mustache::data::type::list);
        Mustache::data class_defines(Mustache::data::type::list);

        auto source_root = GlobalConfig::Get().m_source_root;
        std::string rela_path = Utils::makeRelativePath(source_root, path);
        include_headfiles.push_back(Mustache::data("headfile_item", rela_path));

        std::map<std::string, bool> class_names;
        // class defs
        for (auto class_temp : schema.classes)
        {
            if (!class_temp->shouldCompile())
                continue;

            class_names.insert_or_assign(class_temp->m_name, false);
            class_names[class_temp->m_name] = true;


            Mustache::data class_def;

            genClassRenderData(class_temp, class_def);

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

        std::string rela_file = Utils::makeRelativePath(source_root, headfile_path);

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
            auto output_dir = GlobalConfig::Get().m_output_dir;
            Utils::saveFile(render_string, output_dir + "/reflection.all.h");
        }
    }

    ReflectionGenerator::~ReflectionGenerator() {}
} // namespace Generator
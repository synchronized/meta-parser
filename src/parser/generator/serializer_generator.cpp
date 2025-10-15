#include "generator/serializer_generator.h"
#include "common/precompiled.h"
#include "language_types/class.h"

namespace Generator
{
    static const std::string single_h_temp_name = "serializer.gen.h";
    static const std::string single_cpp_temp_name = "serializer.gen.cpp";
    static const std::string all_h_temp_name = "serializer.all.h";
    static const std::string all_ipp_temp_name = "serializer.all.ipp";

    SerializerGenerator::SerializerGenerator(
            std::string source_directory,
            std::string template_path,
            std::function<std::string(std::string)> get_include_function) 
            : GeneratorInterface(
                source_directory + "/_generated/serializer", 
                source_directory, 
                template_path,
                get_include_function)
    {
        prepareStatus(m_out_path);
    }

    void SerializerGenerator::prepareStatus(std::string path)
    {
        GeneratorInterface::prepareStatus(path);
        auto tmpMgr = TemplateManager::getInstance();
        tmpMgr->loadTemplates(m_template_path, single_h_temp_name);
        tmpMgr->loadTemplates(m_template_path, single_cpp_temp_name);
        tmpMgr->loadTemplates(m_template_path, all_h_temp_name);
        tmpMgr->loadTemplates(m_template_path, all_ipp_temp_name);
        return;
    }

    std::string SerializerGenerator::processFileName(std::string path, std::string ext_name)
    {
        auto relativeDir = fs::path(path).filename().replace_extension("serializer.gen"+ext_name).string();
        return m_out_path + "/" + relativeDir;
    }
    int SerializerGenerator::generate(std::string path, SchemaMoudle schema)
    {
        std::string headfile_path = processFileName(path, ".h");

        Mustache::data mustache_data;
        Mustache::data include_headfiles(Mustache::data::type::list);
        Mustache::data class_defines(Mustache::data::type::list);

        std::string rela_path = Utils::makeRelativePath(m_root_path, path);
        include_headfiles.push_back(Mustache::data("headfile_item", rela_path));
        for (auto class_temp : schema.classes)
        {
            if (!class_temp->shouldCompileFields())
                continue;

            Mustache::data class_def;
            genClassRenderData(class_temp, class_def);

            // deal base class
            for (int index = 0; index < class_temp->m_base_classes.size(); ++index)
            {
                auto& base_class = class_temp->m_base_classes[index];
                auto include_file = m_get_include_func(base_class->m_name);
                if (!include_file.empty())
                {
                    auto include_file_base = processFileName(include_file, ".h");
                    if (headfile_path != include_file_base)
                    {

                        std::string rela_path = Utils::makeRelativePath(m_root_path, include_file_base);
                        include_headfiles.push_back(Mustache::data("headfile_item", rela_path));
                    }
                }
            }
            for (auto field : class_temp->m_fields)
            {
                if (!field->shouldCompile())
                    continue;
                // deal vector
                if (field->isVector())
                {
                    auto include_file = m_get_include_func(field->m_name);
                    if (!include_file.empty())
                    {
                        auto include_file_base = processFileName(include_file, ".h");
                        if (headfile_path != include_file_base)
                        {
                            std::string rela_path = Utils::makeRelativePath(m_root_path, include_file_base);
                            include_headfiles.push_back(Mustache::data(
                                "headfile_item", rela_path));
                        }
                    }
                }
                // deal normal
            }
            class_defines.push_back(class_def);
        }

        mustache_data.set("class_defines", class_defines);
        mustache_data.set("include_headfiles", include_headfiles);

        auto tmpMgr = TemplateManager::getInstance();
        if (tmpMgr->hasTemplate(single_h_temp_name)) {
            std::string render_string =
                tmpMgr->renderByTemplate(single_h_temp_name, mustache_data);
            Utils::saveFile(render_string, headfile_path);
        }

        if (tmpMgr->hasTemplate(single_cpp_temp_name)) {
            std::string render_string =
                tmpMgr->renderByTemplate(single_cpp_temp_name, mustache_data);
            Utils::saveFile(render_string, headfile_path);
        }

        std::string gen_rela_path = Utils::makeRelativePath(m_root_path, headfile_path);
        mustache_data.set("generate_headfile_item", gen_rela_path);
        m_mustache_data_list.push_back(mustache_data);
        return 0;
    }

    void SerializerGenerator::finish()
    {
        Mustache::data mustache_data;
        mustache_data.set("sourefiles", m_mustache_data_list);

        auto tmpMgr = TemplateManager::getInstance();
        if (tmpMgr->hasTemplate(all_h_temp_name)) {
            std::string render_string = tmpMgr->renderByTemplate(all_h_temp_name, mustache_data);
            Utils::saveFile(render_string, m_out_path + "/serializer.all.h");
        }

        if (tmpMgr->hasTemplate(all_ipp_temp_name)) {
            std::string render_string = tmpMgr->renderByTemplate(all_ipp_temp_name, mustache_data);
            Utils::saveFile(render_string, m_out_path + "/serializer.all.ipp");
        }
    }

    SerializerGenerator::~SerializerGenerator() {}
} // namespace Generator

#include "generator/serializer_generator.h"
#include "common/precompiled.h"
#include "language_types/class.h"

namespace Generator
{
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
        TemplateManager::getInstance()->loadTemplates(m_template_path, "allSerializer.h");
        TemplateManager::getInstance()->loadTemplates(m_template_path, "allSerializer.ipp");
        TemplateManager::getInstance()->loadTemplates(m_template_path, "commonSerializerGenFile");
        return;
    }

    std::string SerializerGenerator::processFileName(std::string path)
    {
        auto relativeDir = fs::path(path).filename().replace_extension("serializer.gen.h").string();
        return m_out_path + "/" + relativeDir;
    }
    int SerializerGenerator::generate(std::string path, SchemaMoudle schema)
    {
        std::string file_path = processFileName(path);

        Mustache::data muatache_data;
        Mustache::data include_headfiles(Mustache::data::type::list);
        Mustache::data class_defines(Mustache::data::type::list);

        std::string rela_path = Utils::makeRelativePath(m_root_path, path).string();
        Utils::replaceAll(rela_path, "\\", "/");
        include_headfiles.push_back(Mustache::data("headfile_name", rela_path));
        for (auto class_temp : schema.classes)
        {
            if (!class_temp->shouldCompileFields())
                continue;

            Mustache::data class_def;
            genClassRenderData(class_temp, class_def);

            // deal base class
            for (int index = 0; index < class_temp->m_base_classes.size(); ++index)
            {
                auto include_file = m_get_include_func(class_temp->m_base_classes[index]->name);
                if (!include_file.empty())
                {
                    auto include_file_base = processFileName(include_file);
                    if (file_path != include_file_base)
                    {

                        std::string rela_path = Utils::makeRelativePath(m_root_path, include_file_base).string();
                        Utils::replaceAll(rela_path, "\\", "/");
                        include_headfiles.push_back(Mustache::data("headfile_name", rela_path));
                    }
                }
            }
            for (auto field : class_temp->m_fields)
            {
                if (!field->shouldCompile())
                    continue;
                // deal vector
                if (field->m_type.find("std::vector") == 0)
                {
                    auto include_file = m_get_include_func(field->m_name);
                    if (!include_file.empty())
                    {
                        auto include_file_base = processFileName(include_file);
                        if (file_path != include_file_base)
                        {
                            std::string rela_path = Utils::makeRelativePath(m_root_path, include_file_base).string();
                            Utils::replaceAll(rela_path, "\\", "/");
                            include_headfiles.push_back(Mustache::data(
                                "headfile_name", rela_path));
                        }
                    }
                }
                // deal normal
            }
            class_defines.push_back(class_def);
            m_class_defines.push_back(class_def);

            m_sourcefile_list.emplace_back(class_temp->GetClassFullName());
        }

        muatache_data.set("class_defines", class_defines);
        muatache_data.set("include_headfiles", include_headfiles);
        std::string render_string =
            TemplateManager::getInstance()->renderByTemplate("commonSerializerGenFile", muatache_data);
        Utils::saveFile(render_string, file_path);

        {
            std::string rela_path = Utils::makeRelativePath(m_root_path, file_path).string();
            Utils::replaceAll(rela_path, "\\", "/");
            m_include_headfiles.push_back(
                Mustache::data("headfile_name", rela_path));
        }
        return 0;
    }

    void SerializerGenerator::finish()
    {
        Mustache::data mustache_data;

        Mustache::data sourefile_names    = Mustache::data::type::list;
        for (auto& sourefile_name_upper_camel_case : m_sourcefile_list)
        {
            sourefile_names.push_back(Mustache::data("sourefile_name_upper_camel_case", sourefile_name_upper_camel_case));
        }

        mustache_data.set("class_defines", m_class_defines);
        mustache_data.set("include_headfiles", m_include_headfiles);
        mustache_data.set("sourefile_names", sourefile_names);

        std::string render_string = TemplateManager::getInstance()->renderByTemplate("allSerializer.h", mustache_data);
        Utils::saveFile(render_string, m_out_path + "/all_serializer.h");
        render_string = TemplateManager::getInstance()->renderByTemplate("allSerializer.ipp", mustache_data);
        Utils::saveFile(render_string, m_out_path + "/all_serializer.ipp");
    }

    SerializerGenerator::~SerializerGenerator() {}
} // namespace Generator

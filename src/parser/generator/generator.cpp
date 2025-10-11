#include "common/precompiled.h"

#include "generator/generator.h"
#include "language_types/class.h"

namespace Generator
{
    void GeneratorInterface::prepareStatus(std::string path)
    {
        if (!fs::exists(path))
        {
            fs::create_directories(path);
        }
    }
    void GeneratorInterface::genClassRenderData(std::shared_ptr<Class> class_temp, Mustache::data& class_def)
    {
        class_def.set("class_name", class_temp->getClassName());
        class_def.set("class_qualified_name", class_temp->getClassQualifiedName());
        class_def.set("class_name_with_namespace", class_temp->getClassNameWithNamespace());
        class_def.set("class_need_register", true);

        class_def.set("base_class_size", std::to_string(class_temp->m_base_classes.size()));
        if (class_temp->m_base_classes.size() > 0)
        {
            Mustache::data base_class_defines(Mustache::data::type::list);
            class_def.set("base_class_defined", true);
            for (int index = 0; index < class_temp->m_base_classes.size(); ++index)
            {
                Mustache::data base_class_def;
                base_class_def.set("base_class_name", class_temp->m_base_classes[index]->name);
                base_class_def.set("base_class_index", std::to_string(index));
                base_class_defines.push_back(base_class_def);
            }
            class_def.set("base_class_defines", base_class_defines);
        }

        Mustache::data field_defines = Mustache::data::type::list;
        genClassFieldRenderData(class_temp, field_defines);
        class_def.set("field_defines", field_defines);

        
        Mustache::data method_defines = Mustache::data::type::list;
        genClassMethodRenderData(class_temp, method_defines);
        class_def.set("method_defines", method_defines);
    }
    void GeneratorInterface::genClassFieldRenderData(std::shared_ptr<Class> class_temp, Mustache::data& feild_defines)
    {
        static const std::string vector_prefix = "std::vector<";

        for (auto& field : class_temp->m_fields)
        {
            if (!field->shouldCompile())
                continue;
            Mustache::data filed_define;

            filed_define.set("field_name", field->m_name);
            filed_define.set("field_type_name", field->m_type);
            filed_define.set("field_display_name", field->m_display_name);
            bool is_vector = field->m_type.find(vector_prefix) == 0;
            filed_define.set("field_is_vector", is_vector);
            feild_defines.push_back(filed_define);
        }
    }

    void GeneratorInterface::genClassMethodRenderData(std::shared_ptr<Class> class_temp, Mustache::data& method_defines)
    {
       for (auto& method : class_temp->m_methods)
        {
            if (!method->shouldCompile())
                continue;
            Mustache::data method_define;

            method_define.set("method_name", method->m_name);   
            method_defines.push_back(method_define);
        }
    }
} // namespace Generator

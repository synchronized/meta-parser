#include "common/precompiled.h"

#include "generator/generator.h"
#include "language_types/class.h"

namespace Generator
{


void GeneratorInterface::genClassRenderData(std::shared_ptr<Class> class_temp, Mustache::data& class_def)
{
    class_def.set("class_name", class_temp->m_name);
    class_def.set("class_display_name", class_temp->m_display_name);
    class_def.set("class_qualified_name", class_temp->m_qualified_name);
    class_def.set("class_need_register", true);

    class_def.set("base_class_size", std::to_string(class_temp->m_base_classes.size()));
    if (class_temp->m_base_classes.size() > 0)
    {
        Mustache::data base_class_defines(Mustache::data::type::list);
        class_def.set("base_class_defined", true);
        for (int index = 0; index < class_temp->m_base_classes.size(); ++index)
        {
            Mustache::data base_class_def;
            auto& base_class = class_temp->m_base_classes[index];
            base_class_def.set("base_class_index", std::to_string(index));
            base_class_def.set("base_class_name", base_class->m_name);
            base_class_def.set("base_class_display_name", base_class->m_display_name);
            base_class_def.set("base_class_qualified_name", base_class->m_qualified_name);
            base_class_defines.push_back(base_class_def);
        }
        class_def.set("base_class_defines", base_class_defines);
    }

    Mustache::data field_defines = Mustache::data::type::list;
    genClassFieldRenderData(class_temp, field_defines);
    class_def.set("field_defines", field_defines);
}

void GeneratorInterface::genClassFieldRenderData(std::shared_ptr<Class> class_temp, Mustache::data& feild_defines)
{
    for (auto& field : class_temp->m_fields)
    {
        if (!field->shouldCompile())
            continue;

        Mustache::data filed_define;
        filed_define.set("field_name", field->m_name);
        filed_define.set("field_display_name", field->m_display_name);
        filed_define.set("field_type_name", field->m_type_name);
        filed_define.set("field_type_display_name", field->m_type_display_name);
        filed_define.set("field_type_qualified_name", field->m_type_qualified_name);
        filed_define.set("field_is_vector", field->isVector());
        feild_defines.push_back(filed_define);
    }
}

} // namespace Generator

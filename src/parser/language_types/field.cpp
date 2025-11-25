#include "common/precompiled.h"

#include "language_types/class.h"
#include "language_types/field.h"
#include "common/global_config.h"

Field::Field(const Cursor& cursor, const Namespace& current_namespace, Class* parent) :
    TypeInfo(cursor, current_namespace), 
    m_is_const(cursor.getType().IsConst()), 
    m_parent(parent),
    m_name(cursor.getSpelling()), 
    m_display_name(Utils::getNameWithoutFirstM(m_name)),
    m_type_name(cursor.getType().GetDisplayName()),
    m_type_display_name(cursor.getType().GetDisplayName()),
    m_type_qualified_name(Utils::formatQualifiedName(m_type_name))
{

    Utils::replaceAll(m_name, " ", "");
    Utils::replaceAll(m_display_name, " ", "");
    Utils::replaceAll(m_type_name, " ", "");
    Utils::replaceAll(m_type_display_name, " ", "");
    Utils::replaceAll(m_type_qualified_name, " ", "");

    auto ret_string = Utils::getStringWithoutQuot(m_meta_data.getProperty("default"));
    m_default       = ret_string;
}

bool Field::shouldCompile(void) const { return isAccessible(); }

bool Field::isAccessible(void) const
{
    return ((m_parent->m_meta_data.getFlag(NativeProperty::Fields) ||
             m_parent->m_meta_data.getFlag(NativeProperty::All)) &&
            !m_meta_data.getFlag(NativeProperty::Disable)) ||
           (m_parent->m_meta_data.getFlag(NativeProperty::WhiteListFields) &&
            m_meta_data.getFlag(NativeProperty::Enable));
}

bool Field::isVector(void) const {
    static const std::string vector_prefix = "std::vector<";
    return m_type_name.find(vector_prefix) == 0;
}

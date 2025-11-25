#include "common/precompiled.h"

#include "class.h"
#include "common/global_config.h"

BaseClass::BaseClass(const Cursor& cursor, const Namespace& current_namespace) 
    : TypeInfo(cursor, current_namespace), 
      m_name(cursor.getType().GetDisplayName()),
      m_display_name(cursor.getDisplayName())
{
    if (m_name.find("::") == std::string::npos) {
        if (current_namespace.size() > 0)  {
            m_name = Utils::join(current_namespace, "::") + "::" + m_name;
        }
    }
    Utils::replaceAll(m_name, " ", "");
    Utils::replaceAll(m_display_name, " ", "");
    Utils::replaceAll(m_qualified_name, " ", "");
    m_qualified_name = Utils::formatQualifiedName(m_name);
}

Class::Class(const Cursor& cursor, const Namespace& current_namespace) :
    TypeInfo(cursor, current_namespace), 
    m_name(cursor.getType().GetDisplayName()),
    m_display_name(cursor.getDisplayName()),
    m_qualified_name(Utils::formatQualifiedName(m_name))
{
    Utils::replaceAll(m_name, " ", "");
    Utils::replaceAll(m_display_name, " ", "");
    Utils::replaceAll(m_qualified_name, " ", "");

    for (auto& child : cursor.getChildren())
    {
        switch (child.getKind())
        {
            case CXCursor_CXXBaseSpecifier: {
                auto base_class = new BaseClass(child, current_namespace);

                m_base_classes.emplace_back(base_class);
            }
            break;
            // field
            case CXCursor_FieldDecl:
                m_fields.emplace_back(new Field(child, current_namespace, this));
                break;
            // method
            case CXCursor_CXXMethod:
                m_methods.emplace_back(new Method(child, current_namespace, this));
            default:
                break;
        }
    }
}

bool Class::shouldCompile(void) const { return shouldCompileFields()|| shouldCompileMethods(); }

bool Class::shouldCompileFields(void) const
{
    return m_meta_data.getFlag(NativeProperty::All) || m_meta_data.getFlag(NativeProperty::Fields) ||
           m_meta_data.getFlag(NativeProperty::WhiteListFields);
}

bool Class::shouldCompileMethods(void) const{
    
    return m_meta_data.getFlag(NativeProperty::All) || m_meta_data.getFlag(NativeProperty::Methods) ||
           m_meta_data.getFlag(NativeProperty::WhiteListMethods);
}

bool Class::isAccessible(void) const { return m_enabled; }
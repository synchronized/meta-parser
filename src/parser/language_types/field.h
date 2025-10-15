#pragma once

#include "type_info.h"

class Class;

class Field : public TypeInfo
{

public:
    Field(const Cursor& cursor, const Namespace& current_namespace, Class* parent = nullptr);

    virtual ~Field(void) {}

    bool shouldCompile(void) const;

public:
    bool m_is_const;

    Class* m_parent;

    std::string m_name;
    std::string m_display_name;

    std::string m_type_name;
    std::string m_type_display_name;
    std::string m_type_qualified_name;

    std::string m_default;

    bool isAccessible(void) const;
    bool isVector(void) const;
};
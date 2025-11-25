#pragma once

#include "common/precompiled.h"

#include "common/namespace.h"
#include "common/schema_module.h"

#include "cursor/cursor.h"

#include "generator/generator.h"
#include "template_manager/template_manager.h"
#include "parser/options.h"

class Class;

class MetaParser
{
public:
    static void prepare(void);

    MetaParser(Options& options);
    ~MetaParser(void);
    void finish(void);
    int  parse(void);
    void generateFiles(void);
    void dump();

private:

    std::vector<std::string> m_work_paths;

    CXIndex           m_index;
    CXTranslationUnit m_translation_unit;

    std::unordered_map<std::string, std::string>  m_type_table;
    std::unordered_map<std::string, SchemaMoudle> m_schema_modules;

    std::vector<Generator::GeneratorInterface*> m_generators;

    bool m_is_show_errors;
    Options m_options;
    std::vector<std::string> m_include_list;

private:
    bool        generateIndex(void);
    void        buildClassAST(const Cursor& cursor, Namespace& current_namespace);
    std::string getIncludeFile(std::string name);
    void genClassRenderData(std::shared_ptr<Class> class_temp, Mustache::data& class_def);
    void genClassFieldRenderData(std::shared_ptr<Class> class_temp, Mustache::data& feild_defines);
};

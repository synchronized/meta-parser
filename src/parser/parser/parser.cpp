#include "common/precompiled.h"
#include "common/global_config.h"
#include "language_types/class.h"
#include "generator/reflection_generator.h"
#include "generator/serializer_generator.h"
#include "parser/parser.h"

#define RECURSE_NAMESPACES(kind, cursor, method, namespaces) \
    { \
        if (kind == CXCursor_Namespace) \
        { \
            auto display_name = cursor.getDisplayName(); \
            if (!display_name.empty()) \
            { \
                namespaces.emplace_back(display_name); \
                method(cursor, namespaces); \
                namespaces.pop_back(); \
            } \
        } \
    }

#define TRY_ADD_LANGUAGE_TYPE(handle, container) \
    { \
        if (handle->shouldCompile()) \
        { \
            auto file = handle->getSourceFile(); \
            m_schema_modules[file].container.emplace_back(handle); \
            m_type_table[handle->m_display_name] = file; \
        } \
    }

void MetaParser::prepare(void) { }

std::string MetaParser::getIncludeFile(std::string name)
{
    auto iter = m_type_table.find(name);
    return iter == m_type_table.end() ? std::string() : iter->second;
}

MetaParser::MetaParser(Options& options) 
        : m_options(options),
          m_index(nullptr), 
          m_translation_unit(nullptr)
{

    m_generators.emplace_back(new Generator::SerializerGenerator(
        std::bind(&MetaParser::getIncludeFile, this, std::placeholders::_1)));
    m_generators.emplace_back(new Generator::ReflectionGenerator(
        std::bind(&MetaParser::getIncludeFile, this, std::placeholders::_1)));
}

MetaParser::~MetaParser(void)
{
    for (auto item : m_generators)
    {
        delete item;
    }
    m_generators.clear();

    if (m_translation_unit)
        clang_disposeTranslationUnit(m_translation_unit);

    if (m_index)
        clang_disposeIndex(m_index);
}

void MetaParser::finish(void)
{
    for (auto generator_iter : m_generators)
    {
        generator_iter->finish();
    }
}

//生成文件
bool MetaParser::generateIndex()
{
    bool result = true;

    std::fstream header_list_file(m_options.header_list_file, std::ios::in);

    if (header_list_file.fail())
    {
        std::cout << "Could not load header_list_file: " << m_options.header_list_file << std::endl;
        return false;
    }

    std::vector<std::string> header_list;
    {
        std::string header_file;

        while (std::getline( header_list_file, header_file )) {
            if (!header_file.empty()) {
                header_list.push_back(header_file);
            }
        }
    }

    std::fstream include_file;

    std::string generate_entry_file = m_options.generate_entry_file;
    include_file.open(generate_entry_file, std::ios::out);
    if (!include_file.is_open())
    {
        std::cout << "Could not open the Source Include file: " << generate_entry_file << std::endl;
        return false;
    }

    std::cout << "Generating the Source Include file: " << generate_entry_file << std::endl;

    std::string output_filename = Utils::getFileName(generate_entry_file);

    if (output_filename.empty())
    {
        output_filename = "META_INPUT_HEADER_H";
    }
    else
    {
        Utils::replace(output_filename, ".", "_");
        Utils::replace(output_filename, " ", "_");
        Utils::toUpper(output_filename);
    }
    include_file << "#ifndef __" << output_filename << "__" << std::endl;
    include_file << "#define __" << output_filename << "__" << std::endl;

    for (auto include_item : header_list)
    {
        std::string temp_string(include_item);
        Utils::replace(temp_string, '\\', '/');
        include_file << "#include  \"" << temp_string << "\"" << std::endl;
    }

    include_file << "#endif" << std::endl;
    include_file.close();
    return result;
}

int MetaParser::parse(void)
{
    if (!generateIndex())
    {
        std::cerr << "generate index file error! " << std::endl;
        return -1;
    }

    std::string input_path = m_options.generate_entry_file;
    if (!fs::exists(input_path))
    {
        std::cerr << input_path << " is not exist" << std::endl;
        return -2;
    }

    std::vector<const char *> arguments;

    for (auto &argument : m_options.arguments) 
    {
        arguments.emplace_back( argument.c_str( ) );
    }

    if (m_options.verbose)
    {
        for (auto *argument : arguments) {
            std::cout << argument << std::endl;
        }
    }

    std::cerr << "Parsing the whole project..." << std::endl;

    m_index = clang_createIndex(true, m_options.verbose ? 1 : 0);

    m_translation_unit = clang_createTranslationUnitFromSourceFile(
        m_index, input_path.c_str(), 
        static_cast<int>(arguments.size()), arguments.data(), 0, nullptr);

    auto cursor = clang_getTranslationUnitCursor(m_translation_unit);

    Namespace temp_namespace;

    buildClassAST(cursor, temp_namespace);

    temp_namespace.clear();

    return 0;
}

void MetaParser::generateFiles(void)
{
    std::cerr << "Start generate runtime schemas(" << m_schema_modules.size() << ")..." << std::endl;

    std::string output_dir = GlobalConfig::Get().m_output_dir;
    if (!fs::exists(output_dir))
    {
        fs::create_directories(output_dir);
    }

    for (auto& [path, schema] : m_schema_modules)
    {
        for (auto& generator_iter : m_generators)
        {
            generator_iter->generate(path, schema);
        }
    }

    finish();
}

void MetaParser::buildClassAST(const Cursor& cursor, Namespace& current_namespace)
{
    for (auto& child : cursor.getChildren())
    {
        auto kind = child.getKind();

        // actual definition and a class or struct
        if (child.isDefinition() && (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl))
        {
            auto class_ptr = std::make_shared<Class>(child, current_namespace);

            TRY_ADD_LANGUAGE_TYPE(class_ptr, classes);
        }
        else
        {
            RECURSE_NAMESPACES(kind, child, buildClassAST, current_namespace);
        }
    }
}

void MetaParser::dump()
{
    std::cout << "MetaParser.dump()" << std::endl;
    std::cout << "m_type_table: " << m_type_table.size() << std::endl;
    for (auto& [key, val] : m_type_table) {
        std::cout << "    " << key << ": " << val << std::endl;
    }
    std::cout << "m_schema_modules: " << m_schema_modules.size() << std::endl;
    for (auto& [key, val] : m_schema_modules) {
        std::cout << "    " << key << ":" << std::endl;
        std::cout << "      - name : " << val.name << std::endl;
        std::cout << "        classes : " << val.classes.size() << std::endl;
        for (auto& clazz : val.classes) {
            std::cout << "          - name : " << clazz->m_name << std::endl;
            std::cout << "            display_name : " << clazz->m_display_name << std::endl;
            std::cout << "            qualified_name : " << clazz->m_qualified_name << std::endl;
            std::cout << "            base_class : " << clazz->m_base_classes.size() << std::endl;
            for (auto base_class : clazz->m_base_classes) {
                std::cout << "              - name : " << base_class->m_name << std::endl;
                std::cout << "                display_name : " << base_class->m_display_name << std::endl;
                std::cout << "                qualified_name : " << base_class->m_qualified_name << std::endl;
            }
            std::cout << "            field : " << clazz->m_fields.size() << std::endl;
            for (auto field : clazz->m_fields) {
                std::cout << "              - name : " << field->m_name << std::endl;
                std::cout << "                display_name : " << field->m_display_name << std::endl;
                std::cout << "                type_name : " << field->m_type_name << std::endl;
                std::cout << "                type_display_name : " << field->m_type_display_name << std::endl;
                std::cout << "                type_qualified_name : " << field->m_type_qualified_name << std::endl;
            }
        }
    }
}
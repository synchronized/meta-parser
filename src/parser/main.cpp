#include <argparse/argparse.hpp>

#include "common/precompiled.h"
#include "common/global_config.h"
#include "parser/parser.h"
#include "parser/options.h"

int parse(Options& options);

int main(int argc, char* argv[])
{
    Options options;

    argparse::ArgumentParser program("MetaParser");

    program.add_argument("--target-name")
        .help("Target name (currently only used for detailed log output)")
        .store_into(options.target_name);

    program.add_argument("--remove-namespace-prefix")
        .help("A namespace prefix means that all types that need to be processed will have their corresponding namespace removed if they are within the current namespace.")
        .store_into(options.remove_namespace_prefix);

    program.add_argument("--source-root")
        .help("The root path of the source file, used to generate relative paths based on that path.")
        .required()
        .store_into(options.source_root);

    program.add_argument("--output-dir")
        .help("Save the generated file path (must be relative to --source-root).")
        .required()
        .store_into(options.output_dir);

    program.add_argument("--template-dir")
        .help("Template storage path")
        .required()
        .store_into(options.template_dir);

    program.add_argument("--header-list-file")
        .help("A file path, where each line in the file represents the path to a header file that needs to be analyzed.")
        .required()
        .store_into(options.header_list_file);

    program.add_argument("--generate-entry-file")
        .help("The generated header entry file will contain all header files from the `--header-list-file`.")
        .required()
        .store_into(options.generate_entry_file);

    program.add_argument("--includes-file")
        .help("A file path, where each line in the file stores the path to a header file that needs to be included.")
        .required()
        .store_into(options.includes_file);

    program.add_argument("--verbose")
        .help("Show details")
        .store_into(options.verbose);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    if (options.verbose) {
        options.dump();
    }

    if (fs::path(options.output_dir).is_absolute()) {
        std::cerr << "output_dir must be relative path be source_root" << std::endl;
        return 1;
    }

    auto start_time = std::chrono::system_clock::now();
    int  result     = 0;

    MetaParser::prepare();

    try {
        result = parse(options);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    auto duration_time = std::chrono::system_clock::now() - start_time;
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration_time).count();
    std::cout << "Completed in " << duration_ms << "ms" << std::endl;
    return result;
}

int parse(Options& options) {

    options.arguments = {{
        "-x",
        "c++",
        "-std=c++17",
        "-D__REFLECTION_PARSER__",
        "-DNDEBUG",
        "-D__clang__",
        "-w",
        "-MG",
        "-M",
        "-ferror-limit=0",
        "-o clangLog.txt"
    }};

    //将output_dir 转换为绝对路径
    fs::path outputDir = fs::path(options.output_dir);
    options.output_dir = (fs::path(options.source_root) / outputDir).string();

    //处理include列表
    if (fs::exists(options.includes_file)) 
    {
        std::ifstream includesFile( options.includes_file );

        std::string include;

        while (std::getline( includesFile, include )) {
            if (!include.empty()) {
                options.arguments.emplace_back( "-I"+ include );
            }
        }
    }

    GlobalConfig::Get().m_namespace_prefix = options.remove_namespace_prefix;
    GlobalConfig::Get().m_template_root = options.template_dir;
    GlobalConfig::Get().m_source_root = options.source_root;
    GlobalConfig::Get().m_output_dir = options.output_dir;

    MetaParser parser(options);

    int result = parser.parse();
    std::cerr << "---3---" << std::endl;
    if (0 != result)
    {
        return result;
    }

    if (options.verbose) {
        parser.dump();
    }

    parser.generateFiles();

    return 0;
}

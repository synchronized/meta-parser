#include <argparse/argparse.hpp>

#include "common/precompiled.h"
#include "parser/parser.h"

struct Params {
    std::string project_file_name;
    std::string output_file_name;
    std::string project_root;
    std::string template_root;
    std::string sys_include;
    std::string module_name;
    bool show_errors;
};

int parse(Params& params);

int main(int argc, char* argv[])
{
    Params params;

    argparse::ArgumentParser program("MetaParser");

    program.add_argument("--project_file_name")
        .help("need gencode header files in one file(every line one header file")
        .required()
        .store_into(params.project_file_name);
    program.add_argument("--output_file_name")
        .help("output header file name to process list")
        .required()
        .store_into(params.output_file_name);
    program.add_argument("--project_root")
        .help("generate result save path")
        .required()
        .store_into(params.project_root);
    program.add_argument("--template_root")
        .help("template root path")
        .required()
        .store_into(params.template_root);
    program.add_argument("--sys_include")
        .help("system header include path")
        .store_into(params.sys_include);
    program.add_argument("--module_name")
        .help("module name")
        .store_into(params.module_name);
    program.add_argument("--show_errors")
        .help("is need show errors(don't open it)")
        .store_into(params.show_errors);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    auto start_time = std::chrono::system_clock::now();
    int  result     = 0;

    MetaParser::prepare();

    result = parse(params);

    auto duration_time = std::chrono::system_clock::now() - start_time;
    std::cout << "Completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(duration_time).count()
                << "ms" << std::endl;
    return result;
}

int parse(Params& params) {

    std::cout << std::endl;
    std::cout << "Parsing meta data for target \"" << params.module_name << "\"" << std::endl;
    std::fstream input_file;

    MetaParser parser(
        params.project_file_name, 
        params.output_file_name, 
        params.project_root, 
        params.template_root, 
        params.sys_include, 
        params.module_name, 
        params.show_errors);

    std::cout << "Parsing in " << params.project_root << std::endl;
    int result = parser.parse();
    if (0 != result)
    {
        return result;
    }

    parser.generateFiles();

    return 0;
}

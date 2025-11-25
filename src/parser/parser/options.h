#pragma once

#include <string>
#include <iostream>

struct Options {
    std::string target_name;
    std::string remove_namespace_prefix;
    std::string source_root;
    std::string output_dir;
    std::string template_dir;
    std::string header_list_file;
    std::string generate_entry_file;
    std::string includes_file;
    bool verbose;
    std::string project_file_name;
    std::string output_file_name;
    std::string project_root;

    std::vector<std::string> arguments;

    inline void dump() {
        std::cout << std::endl;
        std::cout << "options dump:" << std::endl;
        std::cout << "    target_name                : \"" << target_name << "\"" << std::endl;
        std::cout << "    remove_namespace_prefix    : \"" << remove_namespace_prefix << "\"" << std::endl;
        std::cout << "    source_root                : \"" << source_root << "\"" << std::endl;
        std::cout << "    output_dir                 : \"" << output_dir << "\"" << std::endl;
        std::cout << "    template_dir               : \"" << template_dir << "\"" << std::endl;
        std::cout << "    header_list_file           : \"" << header_list_file << "\"" << std::endl;
        std::cout << "    generate_entry_file        : \"" << generate_entry_file << "\"" << std::endl;
        std::cout << "    includes_file              : \"" << includes_file << "\"" << std::endl;
        std::cout << "    verbose                    : \"" << verbose << "\"" << std::endl;
    }
};

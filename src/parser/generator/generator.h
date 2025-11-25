#pragma once
#include "common/schema_module.h"

#include <functional>
#include <string>
namespace Generator
{
    class GeneratorInterface
    {
    public:
        GeneratorInterface(std::function<std::string(std::string)> get_include_function) 
            : m_get_include_func(get_include_function)
        {}
        virtual int  generate(std::string path, SchemaMoudle schema) = 0;
        virtual void finish() {};
        virtual void prepareStatus() = 0;

        virtual ~GeneratorInterface() {};

    protected:
        std::function<std::string(std::string)> m_get_include_func;

        void genClassRenderData(std::shared_ptr<Class> class_temp, Mustache::data& class_def);
        void genClassFieldRenderData(std::shared_ptr<Class> class_temp, Mustache::data& feild_defines);

        virtual std::string processFileName(std::string path, std::string ext_name) = 0;
    };
} // namespace Generator

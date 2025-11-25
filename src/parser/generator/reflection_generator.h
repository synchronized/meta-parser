#pragma once
#include "generator/generator.h"
namespace Generator
{
    class ReflectionGenerator : public GeneratorInterface
    {
    public:
        ReflectionGenerator() = delete;
        ReflectionGenerator(
            std::function<std::string(std::string)> get_include_function);
        virtual int  generate(std::string path, SchemaMoudle schema) override;
        virtual void finish() override;
        virtual ~ReflectionGenerator() override;

    protected:
        virtual void        prepareStatus() override;
        virtual std::string processFileName(std::string path, std::string ext_name) override;

    private:
        Mustache::data m_mustache_data_list{Mustache::data::type::list};
    };
} // namespace Generator

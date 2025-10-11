#include <iostream>

#include "runtime/meta/reflection_register.h"
#include "runtime/meta/reflection.hpp"
#include "runtime/meta/serializer_register.h"
#include "runtime/meta/serializer.hpp"

#include "runtime/person.hpp"

int main() {
    using namespace Sandbox;
    reflection::TypeMetaRegister::metaRegister();
    serializer::TypeMetaRegister::metaRegister();

    std::cout << "main" << std::endl;

    {
        auto& meta = reflection::GetByName("Student");
        for (auto field : meta.GetMemberVars()) {
            std::cout << "field.name : " << field.name() << ";" << std::endl;
        }
    }
    {
        auto& meta = reflection::GetByName("Person");
        for (auto field : meta.GetMemberVars()) {
            std::cout << "field.name : " << field.name() << ";" << std::endl;
        }
    }

    Person* tech = new Person(2, "sunday2");
    Student p{
        1, "sunday1", { {"main", "1@qq.com"} }, tech,
    };

    Json json = serializer::Serializer::write(&p);
    std::cout << json.dump() << std::endl;

    json = serializer::Serializer::write(p);
    std::cout << json.dump() << std::endl;
    return 0;
}
#include <iostream>

#include "runtime/meta/reflection_register.h"
#include "runtime/meta/reflection.hpp"
#include "runtime/meta/serializer_register.h"
#include "runtime/meta/serializer.hpp"

int main() {
    reflection::TypeMetaRegister::metaRegister();
    serializer::TypeMetaRegister::metaRegister();

    std::cout << "main" << std::endl;

    auto& meta = reflection::GetByName("Sandbox::Person");
    for (auto field : meta.GetMemberVars()) {
        std::cout << "field.name : " << field.name() << ";" << std::endl;
    }
    return 0;
}
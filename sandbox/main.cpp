#include <iostream>

#include "runtime/meta/reflection_register.h"
#include "runtime/meta/reflection.hpp"

int main() {
    reflection::TypeMetaRegister::metaRegister();

    auto& meta = reflection::GetByName("Person");
    for (auto field : meta.GetMemberVars()) {
        std::cout << "field.name : " << field.name() << ";" << std::endl;
    }
    return 0;
}
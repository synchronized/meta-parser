#pragma once

#include <string>

#include "runtime/meta/reflect.hpp"

namespace Sandbox {

CLASS(Person, Fields) {
public:
    int id;
    std::string name;
};

}

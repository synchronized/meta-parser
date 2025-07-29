#pragma once

#include <string>

#include "runtime/meta/reflection.hpp"

namespace Sandbox {

CLASS(Person, Fields) {
public:
    int id;
    std::string name;
};

}

#include <assert.h>

#include "runtime/meta/reflection.hpp"
#include "runtime/meta/reflection_register.h"

#include "_generated/reflection.all.h"

namespace reflection
{
    void TypeMetaRegister::metaUnregister() { ClearRegistry(); }
} // namespace Reflection

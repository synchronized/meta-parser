#include <assert.h>

#include "runtime/meta/json.h"
#include "runtime/meta/reflection.hpp"
#include "runtime/meta/reflection_register.h"
#include "runtime/meta/serializer.h"

#include "_generated/reflection/all_reflection.h"
#include "_generated/serializer/all_serializer.ipp"

namespace reflection
{
    void TypeMetaRegister::metaUnregister() { ClearRegistry(); }
} // namespace Reflection

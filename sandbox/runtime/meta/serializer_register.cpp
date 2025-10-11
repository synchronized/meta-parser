#include <assert.h>

#include "runtime/meta/json.h"
#include "runtime/meta/serializer.hpp"
#include "runtime/meta/serializer_register.h"

#include "_generated/serializer/serializer.all.h"
#include "_generated/serializer/serializer.all.ipp"

namespace serializer
{
    void TypeMetaRegister::metaUnregister() { ClearRegistry(); }
} // namespace serializer

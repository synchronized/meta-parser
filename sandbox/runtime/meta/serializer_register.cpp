#include <assert.h>

#include "runtime/meta/json.h"
#include "runtime/meta/serializer.hpp"
#include "runtime/meta/serializer_register.h"

#include "_generated/serializer/all_serializer.h"
//#include "_generated/serializer/all_serializer.ipp"

namespace serializer
{
    void TypeMetaRegister::metaUnregister() { ClearRegistry(); }
} // namespace serializer

#ifndef __NAV_MEMORY_HELPER_HPP__
#define __NAV_MEMORY_HELPER_HPP__

#include <Api/Nav_Api.hpp>
#include <Type/Primitive/Nav_Value_Type.hpp>
#include "Nav_Allocation.hpp"
#include "Arena/Nav_Sequential_Arena.hpp"
#include "Arena/Nav_String_Arena.hpp"

class NAV_API MemoryHelper
{
    public :
    static void SetBufferValue(Bool* bufferPtr, Bool setValue, UInt64 bufferSize){for(UInt64 idx = 0ULL; idx < bufferSize; idx++){bufferPtr[idx] = setValue;}}
};

#endif
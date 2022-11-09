#ifdef NAV_BUILD_PLATFORM_MINGW
#define NAV_API_INTERNAL_IMPORT
#endif

#include "Nav_Type.hpp"
#include <Memory/Nav_Memory_Helper.hpp>

inline char* String::Get()
{
    return (char*)(this + 1);
}

Size String::GetCapacity()
{
    return this->_capacity;
}

void String::Set(char* charArray)
{
    if(charArray == NullPtr || this->_capacity < 2){return;}

    char* str = this->Get();
    MemoryHelper::SetBufferValue((Bool*)str, Null, this->_capacity);
    for(Size idx = 0ULL; idx < this->_capacity - 1; idx++)
    {
        char currChar = charArray[idx];
        if(currChar == '\0'){break;}

        str[idx] = currChar;
    }

    str[_capacity - 1] = '\0';
}
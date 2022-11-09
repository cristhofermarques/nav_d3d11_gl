#ifndef __NAV_STRING_HPP__
#define __NAV_STRING_HPP__

#include <Api/Nav_Api.hpp>
#include "Nav_Type.hpp"

class NAV_API String
{
    private :
    Size _capacity;

    public :
    inline char* Get();
    Size GetCapacity();
    void Set(char* strArray);
    Size GetLength();

    public :
    static Bool Create(Size capacity, String* str)
    {
        if(str == NullPtr){return False;}

        str->_capacity = capacity;

        return True;
    }

    static Size GetCharArrayLength(char* charArray)
    {
        if(charArray == NullPtr){return 0ULL;}

        for(Size idx = 0ULL; True; idx++)
        {
            char currChar = charArray[idx];
            if(currChar == '\0'){return idx + 1;}
        }
    }
};

#define DEFINE_STRING_STRUCTURE(size) struct String##size{Size capacity = size;char data[size] = "\0"; inline String* AsString(){return (String*)this;} inline static String##size Empty(){String##size str = String##size{}; str.capacity = (Size)size; str.data[0] = '\0'; return str;}}

DEFINE_STRING_STRUCTURE(8);
DEFINE_STRING_STRUCTURE(16);
DEFINE_STRING_STRUCTURE(32);
DEFINE_STRING_STRUCTURE(64);
DEFINE_STRING_STRUCTURE(128);
DEFINE_STRING_STRUCTURE(256);
DEFINE_STRING_STRUCTURE(512);
DEFINE_STRING_STRUCTURE(1024);
DEFINE_STRING_STRUCTURE(2048);
DEFINE_STRING_STRUCTURE(4096);
DEFINE_STRING_STRUCTURE(8192);

#endif
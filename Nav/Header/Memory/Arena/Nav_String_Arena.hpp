#ifndef __NAV_STRING_ARENA_HPP__
#define __NAV_STRING_ARENA_HPP__

#include <Api/Nav_Api.hpp>
#include "Nav_Sequential_Arena.hpp"
#include "Nav_Type.hpp"

class NAV_API StringArena
{
    private :
    SequentialArena* _str8SeqArena;
    SequentialArena* _str16SeqArena;
    SequentialArena* _str32SeqArena;
    SequentialArena* _str64SeqArena;
    SequentialArena* _str128SeqArena;
    SequentialArena* _str256SeqArena;
    SequentialArena* _str512SeqArena;
    SequentialArena* _str1024SeqArena;
    SequentialArena* _str2048SeqArena;
    SequentialArena* _str4096SeqArena;
    SequentialArena* _str8192SeqArena;

    public :
    String* SubAllocateString(Size capacity);
    String* SubAllocateString(char* charArray);
    void SubDeallocateString(String* str);

    public :
    static StringArena* Create(Size _8 = 1000ULL, Size _16 = 2000ULL, Size _32 = 4000ULL, Size _64 = 2000ULL, Size _128 = 1000ULL, Size _256 = 500ULL, Size _512 = 500ULL, Size _1024 = 250ULL, Size _2048 = 250ULL, Size _4096 = 75ULL, Size _8192 = 75ULL);
    static Bool Destroy(StringArena* strArena);
};

#endif
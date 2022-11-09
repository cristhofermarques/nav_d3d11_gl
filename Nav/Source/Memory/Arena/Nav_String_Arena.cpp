#ifdef NAV_BUILD_PLATFORM_MINGW
#define NAV_API_INTERNAL_IMPORT
#endif

#include <Memory/Nav_Allocation.hpp>
#include <Memory/Arena/Nav_String_Arena.hpp>

String* StringArena::SubAllocateString(Size capacity)
{
    String* str = NullPtr;
    Size strLen = 8ULL;
    if(capacity < 8){str = (String*)this->_str8SeqArena->SubAllocate(); strLen = 8ULL;}
    else if(capacity < 16){str = (String*)this->_str16SeqArena->SubAllocate(); strLen = 16ULL;}
    else if(capacity < 32){str = (String*)this->_str32SeqArena->SubAllocate(); strLen = 32ULL;}
    else if(capacity < 64){str = (String*)this->_str64SeqArena->SubAllocate(); strLen = 64ULL;}
    else if(capacity < 128){str = (String*)this->_str128SeqArena->SubAllocate(); strLen = 128ULL;}
    else if(capacity < 256){str = (String*)this->_str256SeqArena->SubAllocate(); strLen = 256ULL;}
    else if(capacity < 512){str = (String*)this->_str512SeqArena->SubAllocate(); strLen = 512ULL;}
    else if(capacity < 1024){str = (String*)this->_str1024SeqArena->SubAllocate(); strLen = 1024ULL;}
    else if(capacity < 2048){str = (String*)this->_str2048SeqArena->SubAllocate(); strLen = 2048ULL;}
    else if(capacity < 4096){str = (String*)this->_str4096SeqArena->SubAllocate(); strLen = 4096ULL;}
    else if(capacity < 8192){str = (String*)this->_str8192SeqArena->SubAllocate(); strLen = 8192ULL;}

    if(!String::Create(strLen, str)){return NullPtr;}    
    return str;
}

String* StringArena::SubAllocateString(char* charArray)
{
    if(charArray == NullPtr){return NullPtr;}

    int charArrayLen = String::GetCharArrayLength(charArray);
    if(charArrayLen == 0ULL){return NullPtr;}

    String* str = SubAllocateString(charArrayLen);
    if(str == NullPtr){return NullPtr;}

    str->Set(charArray);
    return str;
}

void StringArena::SubDeallocateString(String* str)
{
    if(str == NullPtr){return;}

    Size strCapacity = str->GetCapacity();
    if(strCapacity <= 8ULL){this->_str8SeqArena->SubDeallocate(str);}
    if(strCapacity <= 16ULL){this->_str16SeqArena->SubDeallocate(str);}
    if(strCapacity <= 32ULL){this->_str32SeqArena->SubDeallocate(str);}
    if(strCapacity <= 64ULL){this->_str64SeqArena->SubDeallocate(str);}
    if(strCapacity <= 128ULL){this->_str128SeqArena->SubDeallocate(str);}
    if(strCapacity <= 256ULL){this->_str256SeqArena->SubDeallocate(str);}
    if(strCapacity <= 512ULL){this->_str512SeqArena->SubDeallocate(str);}
    if(strCapacity <= 1024ULL){this->_str1024SeqArena->SubDeallocate(str);}
    if(strCapacity <= 2048ULL){this->_str2048SeqArena->SubDeallocate(str);}
    if(strCapacity <= 4096ULL){this->_str4096SeqArena->SubDeallocate(str);}
    if(strCapacity <= 8192ULL){this->_str8192SeqArena->SubDeallocate(str);}
}

StringArena* StringArena::Create(Size _8, Size _16, Size _32, Size _64, Size _128, Size _256, Size _512, Size _1024, Size _2048, Size _4096, Size _8192)
{
    StringArena* strArena = (StringArena*)MEMORY_ALLOCATE(sizeof(StringArena));
    if(strArena == NullPtr){return NullPtr;}

    Size strSize = sizeof(String);

    strArena->_str8SeqArena = SequentialArena::Create(8ULL + strSize, _8);
    if(strArena->_str8SeqArena == NullPtr){ MEMORY_DEALLOCATE(strArena); return NullPtr;}

    strArena->_str16SeqArena = SequentialArena::Create(16ULL + strSize, _16);
    if(strArena->_str16SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str32SeqArena = SequentialArena::Create(32ULL + strSize, _32);
    if(strArena->_str32SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str64SeqArena = SequentialArena::Create(64ULL + strSize, _64);
    if(strArena->_str64SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str32SeqArena);
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str128SeqArena = SequentialArena::Create(128ULL + strSize, _128);
    if(strArena->_str128SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str64SeqArena);
        SequentialArena::Destroy(strArena->_str32SeqArena);
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str256SeqArena = SequentialArena::Create(256ULL + strSize, _256);
    if(strArena->_str256SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str128SeqArena);
        SequentialArena::Destroy(strArena->_str64SeqArena);
        SequentialArena::Destroy(strArena->_str32SeqArena);
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str512SeqArena = SequentialArena::Create(512ULL + strSize, _512);
    if(strArena->_str512SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str256SeqArena);
        SequentialArena::Destroy(strArena->_str128SeqArena);
        SequentialArena::Destroy(strArena->_str64SeqArena);
        SequentialArena::Destroy(strArena->_str32SeqArena);
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str1024SeqArena = SequentialArena::Create(1024ULL + strSize, _1024);
    if(strArena->_str1024SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str512SeqArena);
        SequentialArena::Destroy(strArena->_str256SeqArena);
        SequentialArena::Destroy(strArena->_str128SeqArena);
        SequentialArena::Destroy(strArena->_str64SeqArena);
        SequentialArena::Destroy(strArena->_str32SeqArena);
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str2048SeqArena = SequentialArena::Create(2048ULL + strSize, _2048);
    if(strArena->_str2048SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str1024SeqArena);
        SequentialArena::Destroy(strArena->_str512SeqArena);
        SequentialArena::Destroy(strArena->_str256SeqArena);
        SequentialArena::Destroy(strArena->_str128SeqArena);
        SequentialArena::Destroy(strArena->_str64SeqArena);
        SequentialArena::Destroy(strArena->_str32SeqArena);
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str4096SeqArena = SequentialArena::Create(4096ULL + strSize, _4096);
    if(strArena->_str4096SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str2048SeqArena);
        SequentialArena::Destroy(strArena->_str1024SeqArena);
        SequentialArena::Destroy(strArena->_str512SeqArena);
        SequentialArena::Destroy(strArena->_str256SeqArena);
        SequentialArena::Destroy(strArena->_str128SeqArena);
        SequentialArena::Destroy(strArena->_str64SeqArena);
        SequentialArena::Destroy(strArena->_str32SeqArena);
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    strArena->_str8192SeqArena = SequentialArena::Create(8192ULL + strSize, _8192);
    if(strArena->_str8192SeqArena == NullPtr)
    {
        SequentialArena::Destroy(strArena->_str4096SeqArena);
        SequentialArena::Destroy(strArena->_str2048SeqArena);
        SequentialArena::Destroy(strArena->_str1024SeqArena);
        SequentialArena::Destroy(strArena->_str512SeqArena);
        SequentialArena::Destroy(strArena->_str256SeqArena);
        SequentialArena::Destroy(strArena->_str128SeqArena);
        SequentialArena::Destroy(strArena->_str64SeqArena);
        SequentialArena::Destroy(strArena->_str32SeqArena);
        SequentialArena::Destroy(strArena->_str16SeqArena);
        SequentialArena::Destroy(strArena->_str8SeqArena);
        MEMORY_DEALLOCATE(strArena);
        return NullPtr;
    }

    return strArena;
}

Bool StringArena::Destroy(StringArena* strArena)
{
    if(strArena == NullPtr){return False;}

    SequentialArena::Destroy(strArena->_str8192SeqArena);
    SequentialArena::Destroy(strArena->_str4096SeqArena);
    SequentialArena::Destroy(strArena->_str2048SeqArena);
    SequentialArena::Destroy(strArena->_str1024SeqArena);
    SequentialArena::Destroy(strArena->_str512SeqArena);
    SequentialArena::Destroy(strArena->_str256SeqArena);
    SequentialArena::Destroy(strArena->_str128SeqArena);
    SequentialArena::Destroy(strArena->_str64SeqArena);
    SequentialArena::Destroy(strArena->_str32SeqArena);
    SequentialArena::Destroy(strArena->_str16SeqArena);
    SequentialArena::Destroy(strArena->_str8SeqArena);

    MEMORY_DEALLOCATE(strArena);

    return True;
}
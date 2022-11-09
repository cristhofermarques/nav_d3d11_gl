#ifdef NAV_BUILD_PLATFORM_MINGW
#define NAV_API_INTERNAL_IMPORT
#endif

//#include <Memory/Nav_Memory.hpp>
#include <Memory/Arena/Nav_Sequential_Arena.hpp>
#include <Memory/Nav_Allocation.hpp>
#include <Type/Primitive/Nav_Cast.hpp>

void* SequentialArena::GetData()
{
    return this + 1;
}

void* SequentialArena::SubAllocate()
{
    UInt64 stride = this->_objSize + sizeof(Bool);
    Bool* data = AS(this->GetData(), Bool*);

    Bool* curr = data + (this->_peek * stride);

    if(! *curr)
    {
        *curr = True;
        this->_peek++;
        return curr + sizeof(Bool);
    }

    for(UInt64 idx = 0; idx < this->_objCapacity; idx++)
    {
        curr = data + (idx * stride);

        if(! *curr)
        {
            *curr = True;
            this->_peek = idx;
            return curr + sizeof(Bool);
        }
    }

    if(this->_peek + 1 > this->_objCapacity){return NullPtr;}
}

void SequentialArena::SubDeallocate(void* objPtr)
{
    Bool* objHead = AS(objPtr, Bool*) - 1;
    Bool* data = AS(this->GetData(), Bool*);
    UInt64 stride = this->_objSize * sizeof(Bool);
    UInt64 objIdx = (objHead - data) / stride;
    Bool* maxData = data + stride * this->_objCapacity;

    if(objPtr >= data && objPtr < maxData && objPtr != NullPtr && *objHead)
    {
        *objHead = False;
    }

    if(objIdx < this->_peek)
    {
        this->_peek = objIdx;
    }
}

SequentialArena* SequentialArena::Create(UInt64 objSize, UInt64 objCapacity)
{
    if(objSize == 0ULL || objCapacity == 0ULL){return NullPtr;}

    UInt64 arenaSize = objSize * objCapacity + sizeof(NavSequentialArena) - sizeof(void*);
    
    SequentialArena* seqArena = (SequentialArena*)MEMORY_ALLOCATE(arenaSize);
    if(seqArena == NullPtr){return NullPtr;}

    //MemoryBufferSetValue(AS(seqArena, Bool*), False, arenaSize);

    for(UInt64 idx = 0ULL; idx < arenaSize; idx++)
    {
        AS_ARRAY(seqArena, UInt8*, idx) = (UInt8)0;
    }

    seqArena->_arenaSize = arenaSize;
    seqArena->_objCapacity = objCapacity;
    seqArena->_objSize = objSize;
    seqArena->_peek = 0ULL;

    return seqArena;
}

Bool SequentialArena::Destroy(SequentialArena* seqArena)
{
    if(seqArena == NullPtr){return False;}
    MEMORY_DEALLOCATE(seqArena);
    return True;
}
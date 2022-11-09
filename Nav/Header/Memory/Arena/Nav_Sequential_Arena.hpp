#ifndef __NAV_SEQUENTIAL_ARENA_HPP__
#define __NAV_SEQUENTIAL_ARENA_HPP__

#include <Api/Nav_Api.hpp>
#include "Nav_Type.hpp"

class SequentialArena
{
    public :

    private :
    UInt64 _arenaSize;
    UInt64 _objSize;
    UInt64 _objCapacity;
    UInt64 _peek;

    public :
    void* GetData();
    void* SubAllocate();
    void SubDeallocate(void* objPtr);

    public :
    static SequentialArena* Create(UInt64 objSize, UInt64 objCapacity);
    static Bool Destroy(SequentialArena* seqArena);
};

struct NavSequentialArenaType
{
    UInt64 arenaSize;
    UInt64 objSize;
    UInt64 objCapacity;
    UInt64 peek;
    void* data;
};

typedef struct NavSequentialArenaType NavSequentialArena;

NAV_API NavSequentialArena* Nav_Memory_Arena_Sequential_Create(UInt64 objSize, UInt64 objCapacity);
NAV_API NavSequentialArena* Nav_Memory_Arena_Sequential_Destroy(UInt64 objSize, UInt64 objCapacity);

NAV_API void* Nav_Memory_Arena_Sequential_SubAllocate(NavSequentialArena* seqArena);
NAV_API Bool Nav_Memory_Arena_Sequential_SubDeallocate(NavSequentialArena* seqArena, void* ptr);

#endif
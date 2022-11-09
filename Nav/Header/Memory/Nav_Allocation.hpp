#ifndef __NAV_ALLOCATION_HPP__
#define __NAV_ALLOCATION_HPP__

#include "Nav_Type.hpp"
#include <stdlib.h>

#define MEMORY_ALLOCATE(bufferSize) malloc(bufferSize)

#define MEMORY_REALLOCATE(bufferPointer, newBufferSize) realloc(bufferPointer, newBufferSize)

#define MEMORY_DEALLOCATE(bufferPointer) if(bufferPointer != NullPtr){free(bufferPointer); bufferPointer = NullPtr;}

#endif
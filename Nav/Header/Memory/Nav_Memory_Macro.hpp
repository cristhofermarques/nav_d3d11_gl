#ifndef __NAV_MEMORY_MACRO_HPP__
#define __NAV_MEMORY_MACRO_HPP__

#include <Type/Primitive/Nav_Cast.hpp>
#include <Math/Nav_Comparation.hpp>
#include <emmintrin.h>
#include <stdio.h>

#define MEMORY_COPY(srcBufferPtr, destBufferPtr, bufferSize) \
    for(Size i = 0ULL; i < bufferSize; i++) \
    { \
        AS_ARRAY(destBufferPtr, UInt8*, i) = AS_ARRAY(srcBufferPtr, UInt8*, i); \
    } 

#define SET_BUFFER(bufferPtr, bufferSize, val) \
    { \
        Bool isOdd = IS_ODD(bufferSize); \
        Size restSize = bufferSize % sizeof(__m128); \
        UInt8* bufferMaxPtr = bufferPtr + bufferSize; \
        UInt8* maxPtr = bufferMaxPtr - restSize; \
        UInt8* ptr = bufferPtr; \
        for(; ptr < maxPtr; ptr += sizeof(__m128)) \
        { \
            __m128i a = _mm_setzero_si128(); \
            _mm_storeu_si128((__m128i*)ptr, a); \
        } \
        if(isOdd) \
        { \
            for(ptr = ptr; ptr < bufferMaxPtr; ptr++) \
            { \
                *ptr = (UInt8)val; \
            } \
        } \
    }

#endif
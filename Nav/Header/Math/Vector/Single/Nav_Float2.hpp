#ifndef __NAV_FLOAT2_HPP__
#define __NAV_FLOAT2_HPP__

#include <Type/Primitive/Nav_Value_Type.hpp>

#define FLOAT2(x, y) (Float2{x, y})

struct Float2
{
    Float x;
    Float y;

    public :
    inline static Float2 Zero() {return FLOAT2(0.0f, 0.0f);}
    inline static Float2 One() {return FLOAT2(1.0f, 1.0f);}
    inline static Float2 UnitX() {return FLOAT2(1.0f, 0.0f);}
    inline static Float2 UnitY() {return FLOAT2(0.0f, 1.0f);}
};

#endif
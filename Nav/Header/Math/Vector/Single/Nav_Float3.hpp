#ifndef __NAV_FLOAT3_HPP__
#define __NAV_FLOAT3_HPP__

#include <Type/Primitive/Nav_Value_Type.hpp>

#define FLOAT3(x, y, z) (Float3{x, y, z})

struct Float3
{
    Float x;
    Float y;
    Float z;

    public :
    inline static Float3 Zero() {return FLOAT3(0.0f, 0.0f, 0.0f);}
    inline static Float3 One() {return FLOAT3(1.0f, 1.0f, 1.0f);}
    inline static Float3 UnitX() {return FLOAT3(1.0f, 0.0f, 0.0f);}
    inline static Float3 UnitY() {return FLOAT3(0.0f, 1.0f, 0.0f);}
    inline static Float3 UnitZ() {return FLOAT3(0.0f, 0.0f, 1.0f);}
};


#endif
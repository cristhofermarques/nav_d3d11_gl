#ifndef __NAV_FLOAT4_HPP__
#define __NAV_FLOAT4_HPP__

#include <Type/Primitive/Nav_Value_Type.hpp>

#define FLOAT4(x, y, z, w) (Float4{x, y, z, w})

struct Float4
{
    Float x;
    Float y;
    Float z;
    Float w;

    public :
    Float* AsArray()
    {
        Float arr[4] = {x, y, z, w};
        return arr;
    }

    public :
    inline static Float4 Zero() {return FLOAT4(0.0f, 0.0f, 0.0f, 0.0f);}
    inline static Float4 One() {return FLOAT4(1.0f, 1.0f, 1.0f, 1.0f);}
    inline static Float4 UnitX() {return FLOAT4(1.0f, 0.0f, 0.0f, 0.0f);}
    inline static Float4 UnitY() {return FLOAT4(0.0f, 1.0f, 0.0f, 0.0f);}
    inline static Float4 UnitZ() {return FLOAT4(0.0f, 0.0f, 1.0f, 0.0f);}
    inline static Float4 UnitW() {return FLOAT4(0.0f, 0.0f, 0.0f, 1.0f);}
};


#endif
#ifndef __NAV_FLOAT4_HPP__
#define __NAV_FLOAT4_HPP__

#include <Type/Nav_Value_Type.hpp>

#define FLOAT4(x, y, z, w) (Float4{x, y, z, w})

struct Float4
{
    Single x;
    Single y;
    Single z;
    Single w;

    public :
    inline static Float4 One() {return FLOAT4(1.0f, 1.0f, 1.0f, 1.0f);}
};


#endif
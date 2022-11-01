#ifndef __NAV_FLOAT3_HPP__
#define __NAV_FLOAT3_HPP__

#include <Type/Nav_Value_Type.hpp>

#define FLOAT3(x, y, z) (Float3{x, y, z})

struct Float3
{
    Single x;
    Single y;
    Single z;

    public :
    inline static Float3 One() {return FLOAT3(1.0f, 1.0f, 1.0f);}
};


#endif
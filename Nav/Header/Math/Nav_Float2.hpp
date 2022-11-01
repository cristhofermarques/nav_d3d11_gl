#ifndef __NAV_FLOAT2_HPP__
#define __NAV_FLOAT2_HPP__

#include <Type/Nav_Value_Type.hpp>

#define FLOAT2(x, y) (Float2{x, y})

struct Float2
{
    Single x;
    Single y;

    public :
    inline static Float2 One() {return FLOAT2(1.0f, 1.0f);}
};

#endif
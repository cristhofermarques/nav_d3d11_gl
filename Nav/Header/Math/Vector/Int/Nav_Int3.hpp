#ifndef __NAV_INT3_HPP__
#define __NAV_INT3_HPP__

#include <Api/Nav_Api.hpp>
#include "Nav_Type.hpp"

#define INT3(x, y, z) (Int3{x, y, z})

struct Int3
{
    Int32 x;
    Int32 y;
    Int32 z;

    public :
    inline static Int3 One() {return INT3(1, 1, 1);};
};

#endif
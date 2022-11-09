#ifndef __NAV_INT2_HPP__
#define __NAV_INT2_HPP__

#include <Api/Nav_Api.hpp>
#include "Nav_Type.hpp"

#define INT2(x, y) (Int2{x, y})

struct Int2 
{
    Int32 x;
    Int32 y;

    public :
    inline static Int2 One() {return INT2(1, 1);};
};

#endif
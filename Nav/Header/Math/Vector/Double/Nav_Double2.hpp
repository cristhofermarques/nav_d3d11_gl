#ifndef __NAV_DOUBLE2_HPP__
#define __NAV_DOUBLE2_HPP__

#include <Type/Primitive/Nav_Value_Type.hpp>

#define DOUBLE2(x, y) (Double2{x, y})

struct Double2
{
    Double x;
    Double y;

    public :
    inline static Double2 Zero() {return DOUBLE2(0.0f, 0.0f);}
    inline static Double2 One() {return DOUBLE2(1.0f, 1.0f);}
    inline static Double2 UnitX() {return DOUBLE2(1.0f, 0.0f);}
    inline static Double2 UnitY() {return DOUBLE2(0.0f, 1.0f);}
};


#endif
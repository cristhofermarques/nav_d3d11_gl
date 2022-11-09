#ifndef __NAV_DOUBLE3_HPP__
#define __NAV_DOUBLE3_HPP__

#include <Type/Primitive/Nav_Value_Type.hpp>

#define DOUBLE3(x, y, z) (Double3{x, y, z})

struct Double3
{
    Double x;
    Double y;
    Double z;

    public :
    inline static Double3 Zero() {return DOUBLE3(0.0f, 0.0f, 0.0f);}
    inline static Double3 One() {return DOUBLE3(1.0f, 1.0f, 1.0f);}
    inline static Double3 UnitX() {return DOUBLE3(1.0f, 0.0f, 0.0f);}
    inline static Double3 UnitY() {return DOUBLE3(0.0f, 1.0f, 0.0f);}
    inline static Double3 UnitZ() {return DOUBLE3(0.0f, 0.0f, 1.0f);}
};


#endif
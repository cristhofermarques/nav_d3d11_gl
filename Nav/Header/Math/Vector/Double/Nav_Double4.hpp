#ifndef __NAV_DOUBLE4_HPP__
#define __NAV_DOUBLE4_HPP__

#include <Type/Primitive/Nav_Value_Type.hpp>

#define DOUBLE4(x, y, z, w) (Double4{x, y, z, w})

struct Double4
{
    Double x;
    Double y;
    Double z;
    Double w;

    public :
    inline static Double4 Zero() {return DOUBLE4(0.0f, 0.0f, 0.0f, 0.0f);}
    inline static Double4 One() {return DOUBLE4(1.0f, 1.0f, 1.0f, 1.0f);}
    inline static Double4 UnitX() {return DOUBLE4(1.0f, 0.0f, 0.0f, 0.0f);}
    inline static Double4 UnitY() {return DOUBLE4(0.0f, 1.0f, 0.0f, 0.0f);}
    inline static Double4 UnitZ() {return DOUBLE4(0.0f, 0.0f, 1.0f, 0.0f);}
    inline static Double4 UnitW() {return DOUBLE4(0.0f, 0.0f, 0.0f, 1.0f);}
};


#endif
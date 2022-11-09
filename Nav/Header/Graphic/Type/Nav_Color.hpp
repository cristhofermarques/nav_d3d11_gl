#ifndef __NAV_COLOR_HPP__
#define __NAV_COLOR_HPP__

#include <Api/Nav_Api.hpp>
#include "Nav_Type.hpp"
#include "Nav_Math.hpp"

class Color
{
    public :
    UInt8 r;
    UInt8 g;
    UInt8 b;
    UInt8 a;

    public :
    inline Float4 GetUNorm()
    {
        Float4 color = Float4::Zero();
        color.x = (Float)(this->r) / 255U;
        color.y = (Float)(this->g) / 255U;
        color.z = (Float)(this->b) / 255U;
        color.w = (Float)(this->a) / 255U;
        return color;
    }
};

#endif
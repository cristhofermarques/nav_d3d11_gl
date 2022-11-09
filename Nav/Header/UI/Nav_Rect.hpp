#ifndef __NAV_RECT_HPP__
#define __NAV_RECT_HPP__

#include "Nav_Type.hpp"

struct Rect
{
    Int32 left;
    Int32 top;
    Int32 right;
    Int32 bottom;

    inline Int2 GetTopLeft()
    {
        return INT2(left, top);
    }

    inline Int2 GetTopRight()
    {
        return INT2(right, top);
    }

    inline Int2 GetBottomLeft()
    {
        return INT2(left, bottom);
    }

    inline Int2 GetBottomRight()
    {
        return INT2(right, bottom);
    }

    inline Int2 GetSize()
    {
        return INT2(right - left, bottom - top);
    }

    inline void SetSize(Int32 width, Int32 height)
    {
        right = left + width;
        bottom = top + height;
    }
};

#endif
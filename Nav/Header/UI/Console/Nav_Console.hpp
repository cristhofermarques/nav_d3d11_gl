#ifndef __NAV_CONSOLE_HPP__
#define __NAV_CONSOLE_HPP__

#include <Api/Nav_Api.hpp>
#include <Type/Primitive/Nav_Value_Type.hpp>

class NAV_API Console
{
    public :
    enum PrintColor
    {
        Black = 0U,
        Gray = 1U,
        White = 2U,
        Yellow = 3U,
        Red = 4U,
        Blue = 5U,
        Green = 6U
    };

    public :
    static void SetPrintColor(PrintColor color);
};

#endif
#ifndef __NAV_WIN32_THREAD_HPP__
#define __NAV_WIN32_THREAD_HPP__

#include <Api/Nav_Api.hpp>
#include "Nav_Type.hpp"

class NAV_API Win32Thread
{
    public :
    struct PlatformPart;

    public :
    PlatformPart* GetPlatformPart();

    static Size GetSize();
};

#endif
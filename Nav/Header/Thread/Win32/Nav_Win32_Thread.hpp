#ifndef __NAV_WIN32_THREAD_HPP__
#define __NAV_WIN32_THREAD_HPP__

#include <Api/Nav_Api.h>
#include <Type/Nav_Value_Type.h>

class NAV_API Win32Thread
{
    public :
    struct PlatformPart;

    public :
    PlatformPart* GetPlatformPart();

    static Size GetSize();
};

#endif
#ifndef __NAV_API_HPP__
#define __NAV_API_HPP__

#ifdef NAV_API_EXPORT

    #define NAV_API __declspec(dllexport)

#else 

    #ifdef NAV_BUILD_PLATFORM_MINGW

        #define NAV_API

    #else

        #define NAV_API __declspec(dllimport)

    #endif

#endif

#endif
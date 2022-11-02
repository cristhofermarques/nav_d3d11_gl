#ifndef __NAV_API_HPP__
#define __NAV_API_HPP__

#ifdef NAV_API_EXPORT

#ifdef NAV_BUILD_PLATFORM_MINGW

#ifdef NAV_API_INTERNAL_IMPORT

#define NAV_API

#else

#define NAV_API __declspec(dllexport)

#endif

#endif

#else 

#ifdef NAV_API_INTERNAL_IMPORT

#define NAV_API

#else

#define NAV_API __declspec(dllimport)

#endif

#endif

#endif
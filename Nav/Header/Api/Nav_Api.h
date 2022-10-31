#ifndef __NAV_API_H__
#define __NAV_API_H__

#ifdef NAV_API_EXPORT

#define NAV_API __declspec(dllexport)

#else 

#define NAV_API __declspec(dllimport)

#endif

#endif
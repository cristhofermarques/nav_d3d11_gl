#ifndef __NAV_LIBRARY_HPP__
#define __NAV_LIBRARY_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Primitive/Nav_Value_Type.hpp>

class NAV_API Library
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    public :
    void* GetFunctionAddress(char* functionName);

    public :
    static Bool Load(char* libPath, Library* lib);
    static Bool GetLoaded(char* libName, Library* lib);
    static Bool Unload(Library* lib);
};

#endif
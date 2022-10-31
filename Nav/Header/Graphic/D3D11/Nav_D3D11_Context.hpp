#ifndef __NAV_D3D11_CONTEXT_H__
#define __NAV_D3D11_CONTEXT_H__

#include <Api/Nav_Api.h>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Nav_Value_Type.h>

class NAV_API D3D11Context
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    public :
    static Bool Create(D3D11Context* ctx);
    static Bool Destroy(D3D11Context* ctx);
};

#endif
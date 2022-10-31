#ifndef __NAV__GRAPHIC_CONTEXT_H__
#define __NAV__GRAPHIC_CONTEXT_H__

#include <Api/Nav_Api.h>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Nav_Value_Type.h>

class NAV_API GraphicContext
{
    public :
    union PlatformUnion;

    enum ApiType
    {
        D3D11
    };

    private :
    ApiType _apiType;

    public :
    PlatformUnion* GetPlatformUnion();
    ApiType GetApiType();

    public :
    static Size GetSize();
    static Bool Create(ApiType type, GraphicContext* gfxCtx);
    static Bool Destroy(GraphicContext* gfxCtx);
};

#endif
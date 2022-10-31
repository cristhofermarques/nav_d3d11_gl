#ifndef __NAV_GRAPHIC_CONTEXT_HPP__
#define __NAV_GRAPHIC_CONTEXT_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Nav_Value_Type.hpp>
  
class NAV_API GraphicContext
{
    public :
    DECLARE_PLATFORM_PART_UNION();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    enum ApiType
    {
        D3D11
    };

    private :
    ApiType _apiType;

    public :
    ApiType GetApiType();

    public :
    static Bool Create(ApiType type, GraphicContext* gfxCtx);
    static Bool Destroy(GraphicContext* gfxCtx);
};

#endif
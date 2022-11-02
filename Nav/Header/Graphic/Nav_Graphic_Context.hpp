#ifndef __NAV_GRAPHIC_CONTEXT_HPP__
#define __NAV_GRAPHIC_CONTEXT_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <UI/Window/Nav_Window.hpp>
#include <Type/Nav_Value_Type.hpp>
#include <Math/Nav_Float4.hpp>
  
class NAV_API GraphicContext
{
    public :
    DECLARE_PLATFORM_PART();
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
    Bool Bind(Window* wnd);
    Bool Unbind();

    public :
    void Clear(Float4 color);
    void Present(Bool vSync);

    public :
    static Bool Create(ApiType type, GraphicContext* gfxCtx);
    static Bool Destroy(GraphicContext* gfxCtx);
    static Bool SupportD3D11();
};

#endif
#ifndef __NAV_GRAPHIC_CONTEXT_HPP__
#define __NAV_GRAPHIC_CONTEXT_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <UI/Window/Nav_Window.hpp>
#include <UI/Nav_Rect.hpp>
#include "Nav_Type.hpp"
#include <Math/Vector/Single/Nav_Float4.hpp>
#include <Graphic/Resource/Nav_Mesh.hpp>
  
class NAV_API GraphicContext
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    enum ApiType
    {
        D3D11,
        OpenGL
    };

    typedef UInt8 Primitive;

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
    void SetViewport(Rect rect, Float minDepth = 0.0f, Float maxDepth = 1.0f);

    public :
    static Bool Create(ApiType type, GraphicContext* gfxCtx);
    static Bool Destroy(GraphicContext* gfxCtx);
    static Bool SupportD3D11();
    static Bool SupportOpenGL();
};

#endif
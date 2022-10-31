#ifndef __NAV_GRAPHIC_CONTEXT_PLATFORM_PART_HPP__
#define __NAV_GRAPHIC_CONTEXT_PLATFORM_PART_HPP__

#include <Graphic/Nav_Graphic_Context.hpp>
#include <Graphic/D3D11/Nav_D3D11_Context.hpp>

IMPLEMENT_PLATFORM_PART_UNION(GraphicContext)
{
    D3D11Context d3d11Context;
};
IMPLEMENT_PLATFORM_PART_GET_SIZE(GraphicContext);
IMPLEMENT_GET_PLATFORM_PART(GraphicContext);

#endif
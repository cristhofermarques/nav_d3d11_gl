#pragma once
#ifndef __WIN32_NAV_D3D11_CONTEXT_PLATFORM_PART_HPP__
#define __WIN32_NAV_D3D11_CONTEXT_PLATFORM_PART_HPP__

#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <d3d11.h>

IMPLEMENT_PLATFORM_PART(D3D11Context)
{
    ID3D11Device* d3d11Device;
    ID3D11DeviceContext* d3d11DeviceContext;
    D3D_FEATURE_LEVEL featureLevel;
};

#endif
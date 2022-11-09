#ifndef __NAV_D3D11_TEXTURE2D_HPP__
#define __NAV_D3D11_TEXTURE2D_HPP__

#include <Graphic/Type/Nav_Color.hpp>
#include <Graphic/Type/Image/Nav_Image.hpp>
#include <d3d11.h>

struct D3D11Texture2D
{
    public :
    ID3D11SamplerState* d3d11SamplerState = NullPtr;
    ID3D11ShaderResourceView* d3d11ShaderResourceView = NullPtr;
    ID3D11Texture2D* d3d11Texture2d = NullPtr;

    public :
    Bool Destroy()
    {
        RELEASE(this->d3d11SamplerState);
        RELEASE(this->d3d11ShaderResourceView);
        RELEASE(this->d3d11Texture2d);

        return True;
    }
};

#endif
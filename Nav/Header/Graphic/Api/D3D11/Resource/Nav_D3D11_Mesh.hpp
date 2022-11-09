#ifndef __NAV_D3D11_MESH_HPP__
#define __NAV_D3D11_MESH_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Primitive/Nav_Value_Type.hpp>
#include <Graphic/Resource/Nav_Mesh.hpp>
#include <d3d11.h>

struct D3D11Mesh
{
    public :
    Mesh::Info info;
    ID3D11Buffer* vertexBuffer = NullPtr;
    ID3D11Buffer* indexBuffer = NullPtr;

    Bool Destroy()
    {
        RELEASE(this->indexBuffer);
        RELEASE(this->vertexBuffer);
        this->info.vertexBufferSize = 0ULL;
        this->info.triangleCount = 0ULL;
        return True;
    }
};

#endif
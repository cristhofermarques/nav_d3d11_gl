#ifndef __NAV_D3D11_SHADER_HPP__
#define __NAV_D3D11_SHADER_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Primitive/Nav_Value_Type.hpp>
#include <Graphic/Resource/Nav_Shader.hpp>
#include <d3d11.h>

struct D3D11Shader
{
    public :
    struct Descriptor
    {
        public :
        WChar* vertexShaderPath = NullPtr;
        WChar* geometryShaderPath = NullPtr;
        WChar* pixelShaderPath = NullPtr;
        WChar* hullShaderPath = NullPtr;
        WChar* domainShaderPath = NullPtr;
        WChar* computeShaderPath = NullPtr;
        Shader::Layout layout = Shader::Layout::Empty();

        public :
        inline static Descriptor Empty()
        {
            Descriptor desc = NullStruct;
            desc.layout = Shader::Layout::Empty();
            return desc;
        }
    };

    ID3D11InputLayout* inputLayout = NullPtr;
    ID3D11VertexShader* vertexShader = NullPtr;
    ID3D11GeometryShader* geometryShader = NullPtr;
    ID3D11PixelShader* pixelShader = NullPtr;
    ID3D11HullShader* hullShader = NullPtr;
    ID3D11DomainShader* domainShader = NullPtr;
    ID3D11ComputeShader* computeShader = NullPtr;
    Shader::Layout layout = Shader::Layout::Empty();

    public :
    void Destroy()
    {
        RELEASE(this->inputLayout);
        RELEASE(this->vertexShader);
        RELEASE(this->geometryShader);
        RELEASE(this->pixelShader);
        RELEASE(this->hullShader);
        RELEASE(this->domainShader);
        RELEASE(this->computeShader);
        this->layout = Shader::Layout::Empty();
    }
};

#endif
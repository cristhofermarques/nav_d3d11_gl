#ifndef __NAV_D3D11_CONTEXT_HPP__
#define __NAV_D3D11_CONTEXT_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Math/Vector/Single/Nav_Float4.hpp>
#include <Graphic/Context/Nav_Graphic_Context.hpp>
#include "Nav_UI.hpp"
#include "Nav_Type.hpp"
#include <d3d11.h>

#define RELEASE(obj) if(obj != NullPtr){obj->Release(); obj = NullPtr;}

#include "../Resource/Nav_D3D11_Mesh.hpp"
#include "../Resource/Nav_D3D11_Shader.hpp"
#include "../Resource/Nav_D3D11_Texture2D.hpp"

class NAV_API D3D11Context
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    enum VSyncMode
    {
        Off = 0U,
        Half = 2U,
        On = 1U
    };

    private :
    Bool _isBinded;

    public :
    inline Bool IsBinded()
    {
        return this->_isBinded;
    }
    Bool HasD3DDevice();
    Bool HasD3DContext();
    Bool HasDevice();
    Bool HasAdapter();
    Bool HasFactory();
    Bool HasSwapChain();
    Bool HasRenderTargetView();

    public :
    Bool Bind(Window::PlatformPart* wndPlatPart, Int2 wndSize, Bool wndIsWindowed, UInt32 refreshRateNumerator = 60U, UInt32 refreshRateDenominator = 1U, UInt32 sampleCount = 1U, UInt32 sampleQuality = 0U, UInt32 bufferCount = 2U);
    Bool Unbind();

    public :
    Void Clear(Float4 color);
    Void Present(VSyncMode vSyncMode);
    Void SetViewport(Rect rect, Float minDepth = 0.0f, Float maxDepth = 1.0f);
    
    public :
    Bool CreateMesh(Mesh::Layout meshLayout, void* vertexBuffer, Size vertexBufferSize, Mesh::Triangle* triangleBuffer, Size triangleCount, D3D11Mesh* mesh, D3D11_USAGE vertexBufferUsage = D3D11_USAGE_DEFAULT, UInt32 vertexBufferCpuAccessFlags = 0U, UInt32 vertexBufferMiscFlags = 0U, D3D11_USAGE indexBufferUsage = D3D11_USAGE_DEFAULT, UInt32 indexBufferCpuAccessFlags = 0U, UInt32 indexBufferMiscFlags = 0U);
    Bool CreateShader(D3D11Shader::Descriptor shaderDesc, D3D11Shader* shader, D3D11_INPUT_CLASSIFICATION inputClass = D3D11_INPUT_PER_VERTEX_DATA);
    Bool CreateTexture2D(Image* image, D3D11Texture2D* texture2d, UInt32 mipLevels = 1U, UInt32 arraySize = 1U, DXGI_SAMPLE_DESC sampleDesc = DXGI_SAMPLE_DESC{1, 0}, D3D11_USAGE texture2dUsage = D3D11_USAGE_IMMUTABLE, UInt32 bindFlags = D3D11_BIND_SHADER_RESOURCE, D3D11_FILTER sampleFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_CLAMP, Float mipLodBias = 0.0f, UInt32 maxAnisotropy = 1U, D3D11_COMPARISON_FUNC comparationFunc = D3D11_COMPARISON_NEVER, Color borderColor = Color{0, 0, 0, 0});

    public :
    Bool BindMesh(D3D11Mesh* mesh);
    Bool BindShader(D3D11Shader* shader);
    Bool BindTexture2D(D3D11Texture2D* texture2d);

    public :
    Void Draw(UInt32 idxCount, UInt32 startIdx = 0U, Int32 baseVtxLoc = 0);

    public :
    static Bool Create(D3D11Context* ctx);
    static Bool Destroy(D3D11Context* ctx);
};

#endif
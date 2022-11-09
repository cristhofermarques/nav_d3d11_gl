#include "Nav.hpp"
#include "Nav_D3D11.hpp"
// #include "Nav_OpenGL.hpp"
// #include <Graphic/Type/Image/Nav_Image.hpp>

int main()
{
    Window* wnd = (Window*)MEMORY_ALLOCATE(Window::GetSize());
    GraphicContext* ctx = (GraphicContext*)MEMORY_ALLOCATE(GraphicContext::GetSize());
    
    if(Window::Create("Nav", 600, 480, Window::WindowMode::Windowed, wnd))
    {
        if(GraphicContext::Create(GraphicContext::ApiType::D3D11, ctx))
        {
            if(ctx->Bind(wnd))
            {
                D3D11Context* d3dCtx = (D3D11Context*)ctx->GetPlatformPart();

                Int2 size = wnd->GetClientSize();
                ctx->SetViewport(Rect{0, 0, size.x, size.y});

                Float vtxs[]
                {
                    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                };

                Mesh::Triangle triangles[]
                {
                    {0, 1, 2},
                    {2, 1, 3}
                };
                
                Mesh::Layout layout = Mesh::Layout{};
                layout.Write(VEC3_ELEMENT);
                layout.Write(VEC3_ELEMENT);
                layout.Write(VEC2_ELEMENT);
                
                D3D11Mesh mesh;
                if(d3dCtx->CreateMesh(layout, vtxs, sizeof(vtxs), triangles, ARRAYSIZE(triangles), &mesh))
                {
                    LOG_SUCCESS("mesh");
                }

                Shader::Layout::Element posEl = Shader::Layout::Element::Empty();
                Shader::Layout::Element nrmEl = Shader::Layout::Element::Empty();
                Shader::Layout::Element uvEl = Shader::Layout::Element::Empty();
                posEl.type = VEC3_ELEMENT;
                nrmEl.type = VEC3_ELEMENT;
                uvEl.type = VEC2_ELEMENT;
                posEl.name.AsString()->Set("POS");
                nrmEl.name.AsString()->Set("NRM");
                uvEl.name.AsString()->Set("UV");
                D3D11Shader::Descriptor shaderDesc = D3D11Shader::Descriptor::Empty();
                shaderDesc.layout.Add(&posEl);
                shaderDesc.layout.Add(&nrmEl);
                shaderDesc.layout.Add(&uvEl);
                shaderDesc.vertexShaderPath = L"vtx.cso";
                shaderDesc.pixelShaderPath = L"pxl.cso";

                D3D11Shader shader;
                d3dCtx->CreateShader(shaderDesc, &shader);

                if(d3dCtx->BindMesh(&mesh))
                {
                    LOG_SUCCESS("mesh");
                }
                if(d3dCtx->BindShader(&shader))
                {
                    LOG_SUCCESS("shader");
                }

                Image* img = Image::QOI_FromFile("C:\\Users\\crist\\Downloads\\qoi_test_images\\dice.qoi", 0);
                if(img != NullPtr)
                {
                    LOG_MSG("loaded");
                    D3D11Texture2D texture;
                    if(d3dCtx->CreateTexture2D(img, &texture))
                    {
                        LOG_MSG("tex");

                        d3dCtx->BindTexture2D(&texture);

                        while(wnd->UpdateEvents())
                        {
                            ctx->Clear(FLOAT4(0, 0.1f, 0, 1));
                            d3dCtx->Draw(mesh.info.triangleCount * 3);
                            ctx->Present(True);
                        }

                        texture.Destroy();
                    }

                    MEMORY_DEALLOCATE(img);
                }
                
                shader.Destroy();
                mesh.Destroy();
                ctx->Unbind();
            }

            GraphicContext::Destroy(ctx);
        }
        Window::Destroy(wnd);
    }

    MEMORY_DEALLOCATE(ctx);
    MEMORY_DEALLOCATE(wnd);
}
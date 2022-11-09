#ifndef __NAV_OPENGL_CONTEXT_HPP__
#define __NAV_OPENGL_CONTEXT_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <UI/Window/Nav_Window.hpp>
#include <UI/Nav_Rect.hpp>
#include <Math/Vector/Single/Nav_Float4.hpp>
#include "../GL/Nav_OpenGL_Functions.hpp"
#include "../GL/glcorearb.h"
#include "../Resource/Nav_OpenGL_Mesh.hpp"

class NAV_API OpenGLContext
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    private :
    Bool _isBinded;
    UInt8 _glMajorVersion;
    UInt8 _glMinorVersion;
    UInt8 _colorBits;
    UInt8 _depthBits;
    OpenGLFunctions* _gl;

    public :
    inline Bool GetIsBinded(){return this->_isBinded;}
    inline UInt8 GetGLMajorVersion(){return this->_glMajorVersion;}
    inline UInt8 GetGLMinorVersion(){return this->_glMinorVersion;}
    inline UInt8 GetColorBits(){return this->_colorBits;}
    inline UInt8 GetDepthBits(){return this->_depthBits;}

    public :
    Bool Bind(Window::PlatformPart* wndPlatPart);
    Bool Unbind();
    void* GetFunctionAddress(char* functionName);
    Bool GetFunctions();

    public :
    void Present();
    void Clear(Float4 color);
    void SetViewport(Rect rect, Float minDepth = 0.0f, Float maxDepth = 1.0f);

    public :
    Bool CreateMesh(Mesh::Layout meshLayout, Float* vertexBuffer, Size vertexBufferSize, Mesh::Triangle* triangles, Size triangleCount, OpenGLMesh* mesh, GLenum vertexBufferUsage = GL_STATIC_DRAW, GLenum triangleBufferUsage = GL_STATIC_DRAW);
    Bool DestroyMesh(OpenGLMesh* mesh);

    public :
    static Bool Create(UInt8 glMajorVersion, UInt8 glMinorVersion, OpenGLContext* glCtx, UInt8 colorBits = 32U, UInt8 depthBits = 24U);
    static Bool Destroy(OpenGLContext* glCtx);
};

#endif
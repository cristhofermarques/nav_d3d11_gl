#include <Graphic/Api/OpenGL/Context/Nav_OpenGL_Context.hpp>
#include <Graphic/Api/OpenGL/Context/Win32/Win32_Nav_OpenGL_Context_Platform_Part.hpp>
#include <Memory/Nav_Allocation.hpp>
#include <UI/Window/Win32/Win32_Nav_Window_Platform_Part.hpp>
#include <Debug/Nav_Log.hpp>
#include <Graphic/Api/OpenGL/GL/wglext.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define GL_CALL(func, ...) if(func != NullPtr){func(__VA_ARGS__);}
#define GL_RETURN_CALL(func, ...) func != NullPtr ? func(__VA_ARGS__) : NullPtr;

IMPLEMENT_GET_PLATFORM_PART(OpenGLContext);
IMPLEMENT_PLATFORM_PART_GET_SIZE(OpenGLContext);

Bool OpenGLContext::Bind(Window::PlatformPart* wndPlatPart)
{
    if(this->_isBinded){return False;}

    if(wndPlatPart == NullPtr){return False;}
    GET_PLATFORM_PART_OR_RETURN(False);

    Int32 pxFmtAttribs[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,        
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_COLOR_BITS_ARB, (Int32)this->_colorBits,
        WGL_DEPTH_BITS_ARB, (Int32)this->_depthBits,
        0
    };

    Int32 pxFmt = 0;
    UInt32 pxFmtCount = 0;
    if(! platPart->wglChoosePixelFormatARB(wndPlatPart->hDc, (const int*)pxFmtAttribs, NullPtr, 1, (int*)&pxFmt, (UINT*)&pxFmtCount))
    {
        return False;
    }

    if(GetPixelFormat(wndPlatPart->hDc) != pxFmt)
    {    
        if(! SetPixelFormat(wndPlatPart->hDc, pxFmt, &platPart->pxFmtDesc))
        {
            return False;
        }
    }

    if(wglMakeCurrent(wndPlatPart->hDc, platPart->hGlRc) == FALSE)
    {
        return False;
    }

    this->GetFunctions();

    platPart->hDc = wndPlatPart->hDc;
    this->_isBinded = True;
    return True;
}

Bool OpenGLContext::Unbind()
{
    if(!this->_isBinded){return False;}

    wglMakeCurrent(0, 0);
    this->_isBinded = false;

    return True;
}

void* OpenGLContext::GetFunctionAddress(char* functionName)
{
    if(functionName == NullPtr){return NullPtr;}
    GET_PLATFORM_PART_OR_RETURN(NullPtr);
    if(platPart->glLib == NullPtr){return NullPtr;}

    void* funcPtr = platPart->glLib->GetFunctionAddress(functionName);
    if(funcPtr == NullPtr)
    {
        funcPtr = (void*)wglGetProcAddress(functionName);
    }

    return funcPtr;
}

Bool OpenGLContext::Create(UInt8 glMajorVersion, UInt8 glMinorVersion, OpenGLContext* glCtx, UInt8 colorBits, UInt8 depthBits)
{
    GET_PLATFORM_PART(glCtx, OpenGLContext, False);

    Window* dummyWnd = (Window*)MEMORY_ALLOCATE(Window::GetSize());
    if(dummyWnd == NullPtr){return False;}

    if(!Window::Create("", 0, 0, Window::WindowMode::Borderless, dummyWnd)){return False;}
    GET_PLATFORM_PART(dummyWnd, Window, False);

    PIXELFORMATDESCRIPTOR pxFmtDesc = NullStruct;
    pxFmtDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pxFmtDesc.nVersion = 1;
    pxFmtDesc.dwFlags = PFD_SUPPORT_OPENGL;
    pxFmtDesc.iPixelType = PFD_TYPE_RGBA;
    pxFmtDesc.cColorBits = colorBits;
    pxFmtDesc.cDepthBits = depthBits;
    pxFmtDesc.iLayerType = PFD_MAIN_PLANE;

    Int32 pxFmt = ChoosePixelFormat(dummyWndPlatPart->hDc, &pxFmtDesc);

    if(! pxFmt)
    {
        Window::Destroy(dummyWnd);
        return False;
    }

    if(! SetPixelFormat(dummyWndPlatPart->hDc, pxFmt, &pxFmtDesc))
    {
        Window::Destroy(dummyWnd);
        return False;
    }

    HGLRC hGlRc = wglCreateContext(dummyWndPlatPart->hDc);

    if(! hGlRc)
    {
        Window::Destroy(dummyWnd);
        return False;
    }

    if(! wglMakeCurrent(dummyWndPlatPart->hDc, hGlRc))
    {
        wglDeleteContext(hGlRc);
        Window::Destroy(dummyWnd);
        return False;
    }

    glCtxPlatPart->wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    glCtxPlatPart->wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if(glCtxPlatPart->wglChoosePixelFormatARB == NullPtr || glCtxPlatPart->wglCreateContextAttribsARB == NullPtr)
    {
        wglMakeCurrent(0, 0);
        wglDeleteContext(hGlRc);
        Window::Destroy(dummyWnd);
        return False;
    }

    wglMakeCurrent(0, 0);
    wglDeleteContext(hGlRc);
    Window::Destroy(dummyWnd);

    if(!Window::Create("", 0, 0, Window::WindowMode::Borderless, dummyWnd)){return False;}

    Int32 pxFmtAttribs[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,        
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_COLOR_BITS_ARB, (Int32)colorBits,
        WGL_DEPTH_BITS_ARB, (Int32)depthBits,
        0
    };

    UInt32 pxFmtCount = 0;
    if(! glCtxPlatPart->wglChoosePixelFormatARB(dummyWndPlatPart->hDc, (const int*)pxFmtAttribs, NullPtr, 1, (int*)&pxFmt, (UINT*)&pxFmtCount))
    {
        return False;
    }

    if(! SetPixelFormat(dummyWndPlatPart->hDc, pxFmt, &pxFmtDesc))
    {
        return False;
    }

    Int32 glCtxAttribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, glMajorVersion,
        WGL_CONTEXT_MINOR_VERSION_ARB, glMinorVersion,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB

#ifdef NAV_BUILD_MODE_DEBUG
        | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
        , 0
    };

    hGlRc = glCtxPlatPart->wglCreateContextAttribsARB(dummyWndPlatPart->hDc, NullPtr, (const int*)glCtxAttribs);

    if(! hGlRc)
    {
        return False;
    }

    if(! wglMakeCurrent(dummyWndPlatPart->hDc, hGlRc))
    {
        return False;
    }

    OpenGLFunctions* gl = (OpenGLFunctions*)MEMORY_ALLOCATE(sizeof(OpenGLFunctions));

    Library* glLib = (Library*)MEMORY_ALLOCATE(Library::GetSize());
    if(!Library::GetLoaded("opengl32.dll", glLib) || gl == NullPtr)
    {
        wglDeleteContext(hGlRc);
        MEMORY_DEALLOCATE(glCtxPlatPart->glLib);
        Window::Destroy(dummyWnd);
        MEMORY_DEALLOCATE(dummyWnd);
        return False;
    }

    glCtxPlatPart->glLib = glLib;
    glCtxPlatPart->hGlRc = hGlRc;
    glCtxPlatPart->pxFmtDesc = pxFmtDesc;


    glCtx->_isBinded = False;
    glCtx->_gl = gl;
    glCtx->_glMajorVersion = glMajorVersion;
    glCtx->_glMinorVersion = glMinorVersion;
    glCtx->_colorBits = colorBits;
    glCtx->_depthBits = depthBits;

    if(!wglMakeCurrent(0, 0))
    {
        wglDeleteContext(hGlRc);
        MEMORY_DEALLOCATE(glCtxPlatPart->glLib);
        Window::Destroy(dummyWnd);
        MEMORY_DEALLOCATE(dummyWnd);
        return False;
    }

    Window::Destroy(dummyWnd);
    MEMORY_DEALLOCATE(dummyWnd);
    return True;
}

Bool OpenGLContext::Destroy(OpenGLContext* glCtx)
{
    GET_PLATFORM_PART(glCtx, OpenGLContext, False);

    MEMORY_DEALLOCATE(glCtx->_gl);

    if(glCtxPlatPart->hGlRc != Null)
    {
        wglDeleteContext(glCtxPlatPart->hGlRc);
        glCtxPlatPart->hGlRc = Null;
    }

    if(glCtxPlatPart->glLib != NullPtr)
    {
        MEMORY_DEALLOCATE(glCtxPlatPart->glLib);
    }

    return True;
}

void OpenGLContext::Present()
{
    if(!this->_isBinded){return;}
    GET_PLATFORM_PART_OR_RETURN();

    SwapBuffers(platPart->hDc);
}

void OpenGLContext::Clear(Float4 color)
{
    if(!this->_isBinded){return;}

    this->_gl->glClearColor(color.x, color.y, color.z, color.w);
    this->_gl->glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLContext::SetViewport(Rect rect, Float minDepth, Float maxDepth)
{
    if(! this->_isBinded){return;}
    
    Int2 size = rect.GetSize();
    GL_CALL(this->_gl->glViewport, rect.left, rect.top, size.x, size.y);
    GL_CALL(this->_gl->glDepthRange, (GLdouble)minDepth, (GLdouble)maxDepth);
}

Bool OpenGLContext::CreateMesh(Mesh::Layout meshLayout, Float* vertexBuffer, Size vertexBufferSize, Mesh::Triangle* triangles, Size triangleCount, OpenGLMesh* mesh, GLenum vertexBufferUsage, GLenum triangleBufferUsage)
{
    if(mesh == NullPtr){return False;}

    UInt32 vao = 0U;
    this->_gl->glGenVertexArrays(1, (GLuint*)&vao);
    if(vao == 0U){return False;}
    this->_gl->glBindVertexArray(vao);

    UInt32 vbo = 0U;
    this->_gl->glGenBuffers(1U, (GLuint*)&vbo);
    if(vbo == 0U)
    {
        _gl->glBindVertexArray(Null);
        _gl->glDeleteVertexArrays(1, (GLuint*)vao);
        return False;
    }

    this->_gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    this->_gl->glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexBuffer, vertexBufferUsage);
    
    Size stride = meshLayout.GetVertexSize();
    Size offset = 0ULL;
    for(Size attribIdx = 0ULL; attribIdx < meshLayout.GetElementsCount(); attribIdx++)
    {
        Int32 iAttribIdx = (Int32)attribIdx;
        Mesh::Layout::Element element = meshLayout.GetIndex(iAttribIdx);
        Int32 elementMemberCount = meshLayout.GetElementMemberCount(element);

        this->_gl->glEnableVertexAttribArray(iAttribIdx);
        this->_gl->glVertexAttribPointer(iAttribIdx, elementMemberCount, GL_FLOAT, GL_FALSE, stride, (void*)offset);

        offset += (Size)element;
    }

    UInt32 ibo = 0U;
    this->_gl->glGenBuffers(1U, (GLuint*)&ibo);
    if(ibo == 0U)
    {
        _gl->glBindBuffer(Null, Null);
        _gl->glDeleteBuffers(1U, (GLuint*)&vbo);
        _gl->glBindVertexArray(Null);
        _gl->glDeleteVertexArrays(1, (GLuint*)vao);
        return False;
    }

    this->_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)ibo);
    this->_gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Mesh::Triangle) * triangleCount, (void*)triangles, triangleBufferUsage);

    this->_gl->glBindBuffer(Null, Null);
    this->_gl->glBindVertexArray(Null);

    mesh->vao = vao;
    mesh->vbo = vbo;
    mesh->ibo = ibo;
    mesh->layout = meshLayout;

    return True;
}

Bool OpenGLContext::DestroyMesh(OpenGLMesh* mesh)
{
    if(mesh == NullPtr){return False;}

    if(mesh->ibo != Null){_gl->glDeleteBuffers(1U, (GLuint*)&mesh->ibo); mesh->ibo = Null;}
    if(mesh->vbo != Null){_gl->glDeleteBuffers(1U, (GLuint*)&mesh->vbo); mesh->vbo = Null;}
    if(mesh->vao != Null){_gl->glDeleteVertexArrays(1U, (GLuint*)&mesh->vao); mesh->vao = Null;}

    return True;
}

Bool OpenGLContext::GetFunctions()
{
    this->_gl->glCullFace = (PFNGLCULLFACEPROC) this->GetFunctionAddress("glCullFace");
    this->_gl->glFrontFace = (PFNGLFRONTFACEPROC) this->GetFunctionAddress("glFrontFace");
    this->_gl->glHint = (PFNGLHINTPROC) this->GetFunctionAddress("glHint");
    this->_gl->glLineWidth = (PFNGLLINEWIDTHPROC) this->GetFunctionAddress("glLineWidth");
    this->_gl->glPointSize = (PFNGLPOINTSIZEPROC) this->GetFunctionAddress("glPointSize");
    this->_gl->glPolygonMode = (PFNGLPOLYGONMODEPROC) this->GetFunctionAddress("glPolygonMode");
    this->_gl->glScissor = (PFNGLSCISSORPROC) this->GetFunctionAddress("glScissor");
    this->_gl->glTexParameterf = (PFNGLTEXPARAMETERFPROC) this->GetFunctionAddress("glTexParameterf");
    this->_gl->glTexParameterfv = (PFNGLTEXPARAMETERFVPROC) this->GetFunctionAddress("glTexParameterfv");
    this->_gl->glTexParameteri = (PFNGLTEXPARAMETERIPROC) this->GetFunctionAddress("glTexParameteri");
    this->_gl->glTexParameteriv = (PFNGLTEXPARAMETERIVPROC) this->GetFunctionAddress("glTexParameteriv");
    this->_gl->glTexImage1D = (PFNGLTEXIMAGE1DPROC) this->GetFunctionAddress("glTexImage1D");
    this->_gl->glTexImage2D = (PFNGLTEXIMAGE2DPROC) this->GetFunctionAddress("glTexImage2D");
    this->_gl->glDrawBuffer = (PFNGLDRAWBUFFERPROC) this->GetFunctionAddress("glDrawBuffer");
    this->_gl->glClear = (PFNGLCLEARPROC) this->GetFunctionAddress("glClear");
    this->_gl->glClearColor = (PFNGLCLEARCOLORPROC) this->GetFunctionAddress("glClearColor");
    this->_gl->glClearStencil = (PFNGLCLEARSTENCILPROC) this->GetFunctionAddress("glClearStencil");
    this->_gl->glClearDepth = (PFNGLCLEARDEPTHPROC) this->GetFunctionAddress("glClearDepth");
    this->_gl->glStencilMask = (PFNGLSTENCILMASKPROC) this->GetFunctionAddress("glStencilMask");
    this->_gl->glColorMask = (PFNGLCOLORMASKPROC) this->GetFunctionAddress("glColorMask");
    this->_gl->glDepthMask = (PFNGLDEPTHMASKPROC) this->GetFunctionAddress("glDepthMask");
    this->_gl->glDisable = (PFNGLDISABLEPROC) this->GetFunctionAddress("glDisable");
    this->_gl->glEnable = (PFNGLENABLEPROC) this->GetFunctionAddress("glEnable");
    this->_gl->glFinish = (PFNGLFINISHPROC) this->GetFunctionAddress("glFinish");
    this->_gl->glFlush = (PFNGLFLUSHPROC) this->GetFunctionAddress("glFlush");
    this->_gl->glBlendFunc = (PFNGLBLENDFUNCPROC) this->GetFunctionAddress("glBlendFunc");
    this->_gl->glLogicOp = (PFNGLLOGICOPPROC) this->GetFunctionAddress("glLogicOp");
    this->_gl->glStencilFunc = (PFNGLSTENCILFUNCPROC) this->GetFunctionAddress("glStencilFunc");
    this->_gl->glStencilOp = (PFNGLSTENCILOPPROC) this->GetFunctionAddress("glStencilOp");
    this->_gl->glDepthFunc = (PFNGLDEPTHFUNCPROC) this->GetFunctionAddress("glDepthFunc");
    this->_gl->glPixelStoref = (PFNGLPIXELSTOREFPROC) this->GetFunctionAddress("glPixelStoref");
    this->_gl->glPixelStorei = (PFNGLPIXELSTOREIPROC) this->GetFunctionAddress("glPixelStorei");
    this->_gl->glReadBuffer = (PFNGLREADBUFFERPROC) this->GetFunctionAddress("glReadBuffer");
    this->_gl->glReadPixels = (PFNGLREADPIXELSPROC) this->GetFunctionAddress("glReadPixels");
    this->_gl->glGetBooleanv = (PFNGLGETBOOLEANVPROC) this->GetFunctionAddress("glGetBooleanv");
    this->_gl->glGetDoublev = (PFNGLGETDOUBLEVPROC) this->GetFunctionAddress("glGetDoublev");
    this->_gl->glGetError = (PFNGLGETERRORPROC) this->GetFunctionAddress("glGetError");
    this->_gl->glGetFloatv = (PFNGLGETFLOATVPROC) this->GetFunctionAddress("glGetFloatv");
    this->_gl->glGetIntegerv = (PFNGLGETINTEGERVPROC) this->GetFunctionAddress("glGetIntegerv");
    this->_gl->glGetString = (PFNGLGETSTRINGPROC) this->GetFunctionAddress("glGetString");
    this->_gl->glGetTexImage = (PFNGLGETTEXIMAGEPROC) this->GetFunctionAddress("glGetTexImage");
    this->_gl->glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) this->GetFunctionAddress("glGetTexParameterfv");
    this->_gl->glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) this->GetFunctionAddress("glGetTexParameteriv");
    this->_gl->glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC) this->GetFunctionAddress("glGetTexLevelParameterfv");
    this->_gl->glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) this->GetFunctionAddress("glGetTexLevelParameteriv");
    this->_gl->glIsEnabled = (PFNGLISENABLEDPROC) this->GetFunctionAddress("glIsEnabled");
    this->_gl->glDepthRange = (PFNGLDEPTHRANGEPROC) this->GetFunctionAddress("glDepthRange");
    this->_gl->glViewport = (PFNGLVIEWPORTPROC) this->GetFunctionAddress("glViewport");
    this->_gl->glDrawArrays = (PFNGLDRAWARRAYSPROC) this->GetFunctionAddress("glDrawArrays");
    this->_gl->glDrawElements = (PFNGLDRAWELEMENTSPROC) this->GetFunctionAddress("glDrawElements");
    this->_gl->glGetPointerv = (PFNGLGETPOINTERVPROC) this->GetFunctionAddress("glGetPointerv");
    this->_gl->glPolygonOffset = (PFNGLPOLYGONOFFSETPROC) this->GetFunctionAddress("glPolygonOffset");
    this->_gl->glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC) this->GetFunctionAddress("glCopyTexImage1D");
    this->_gl->glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) this->GetFunctionAddress("glCopyTexImage2D");
    this->_gl->glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC) this->GetFunctionAddress("glCopyTexSubImage1D");
    this->_gl->glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) this->GetFunctionAddress("glCopyTexSubImage2D");
    this->_gl->glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC) this->GetFunctionAddress("glTexSubImage1D");
    this->_gl->glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) this->GetFunctionAddress("glTexSubImage2D");
    this->_gl->glBindTexture = (PFNGLBINDTEXTUREPROC) this->GetFunctionAddress("glBindTexture");
    this->_gl->glDeleteTextures = (PFNGLDELETETEXTURESPROC) this->GetFunctionAddress("glDeleteTextures");
    this->_gl->glGenTextures = (PFNGLGENTEXTURESPROC) this->GetFunctionAddress("glGenTextures");
    this->_gl->glIsTexture = (PFNGLISTEXTUREPROC) this->GetFunctionAddress("glIsTexture");
    this->_gl->glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) this->GetFunctionAddress("glDrawRangeElements");
    this->_gl->glTexImage3D = (PFNGLTEXIMAGE3DPROC) this->GetFunctionAddress("glTexImage3D");
    this->_gl->glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) this->GetFunctionAddress("glTexSubImage3D");
    this->_gl->glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC) this->GetFunctionAddress("glCopyTexSubImage3D");
    this->_gl->glActiveTexture = (PFNGLACTIVETEXTUREPROC) this->GetFunctionAddress("glActiveTexture");
    this->_gl->glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC) this->GetFunctionAddress("glSampleCoverage");
    this->_gl->glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) this->GetFunctionAddress("glCompressedTexImage3D");
    this->_gl->glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) this->GetFunctionAddress("glCompressedTexImage2D");
    this->_gl->glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) this->GetFunctionAddress("glCompressedTexImage1D");
    this->_gl->glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) this->GetFunctionAddress("glCompressedTexSubImage3D");
    this->_gl->glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) this->GetFunctionAddress("glCompressedTexSubImage2D");
    this->_gl->glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) this->GetFunctionAddress("glCompressedTexSubImage1D");
    this->_gl->glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) this->GetFunctionAddress("glGetCompressedTexImage");
    this->_gl->glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) this->GetFunctionAddress("glBlendFuncSeparate");
    this->_gl->glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC) this->GetFunctionAddress("glMultiDrawArrays");
    this->_gl->glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC) this->GetFunctionAddress("glMultiDrawElements");
    this->_gl->glPointParameterf = (PFNGLPOINTPARAMETERFPROC) this->GetFunctionAddress("glPointParameterf");
    this->_gl->glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) this->GetFunctionAddress("glPointParameterfv");
    this->_gl->glPointParameteri = (PFNGLPOINTPARAMETERIPROC) this->GetFunctionAddress("glPointParameteri");
    this->_gl->glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) this->GetFunctionAddress("glPointParameteriv");
    this->_gl->glBlendColor = (PFNGLBLENDCOLORPROC) this->GetFunctionAddress("glBlendColor");
    this->_gl->glBlendEquation = (PFNGLBLENDEQUATIONPROC) this->GetFunctionAddress("glBlendEquation");
    this->_gl->glGenQueries = (PFNGLGENQUERIESPROC) this->GetFunctionAddress("glGenQueries");
    this->_gl->glDeleteQueries = (PFNGLDELETEQUERIESPROC) this->GetFunctionAddress("glDeleteQueries");
    this->_gl->glIsQuery = (PFNGLISQUERYPROC) this->GetFunctionAddress("glIsQuery");
    this->_gl->glBeginQuery = (PFNGLBEGINQUERYPROC) this->GetFunctionAddress("glBeginQuery");
    this->_gl->glEndQuery = (PFNGLENDQUERYPROC) this->GetFunctionAddress("glEndQuery");
    this->_gl->glGetQueryiv = (PFNGLGETQUERYIVPROC) this->GetFunctionAddress("glGetQueryiv");
    this->_gl->glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC) this->GetFunctionAddress("glGetQueryObjectiv");
    this->_gl->glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC) this->GetFunctionAddress("glGetQueryObjectuiv");
    this->_gl->glBindBuffer = (PFNGLBINDBUFFERPROC) this->GetFunctionAddress("glBindBuffer");
    this->_gl->glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) this->GetFunctionAddress("glDeleteBuffers");
    this->_gl->glGenBuffers = (PFNGLGENBUFFERSPROC) this->GetFunctionAddress("glGenBuffers");
    this->_gl->glIsBuffer = (PFNGLISBUFFERPROC) this->GetFunctionAddress("glIsBuffer");
    this->_gl->glBufferData = (PFNGLBUFFERDATAPROC) this->GetFunctionAddress("glBufferData");
    this->_gl->glBufferSubData = (PFNGLBUFFERSUBDATAPROC) this->GetFunctionAddress("glBufferSubData");
    this->_gl->glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC) this->GetFunctionAddress("glGetBufferSubData");
    this->_gl->glMapBuffer = (PFNGLMAPBUFFERPROC) this->GetFunctionAddress("glMapBuffer");
    this->_gl->glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) this->GetFunctionAddress("glUnmapBuffer");
    this->_gl->glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) this->GetFunctionAddress("glGetBufferParameteriv");
    this->_gl->glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC) this->GetFunctionAddress("glGetBufferPointerv");
    this->_gl->glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) this->GetFunctionAddress("glBlendEquationSeparate");
    this->_gl->glDrawBuffers = (PFNGLDRAWBUFFERSPROC) this->GetFunctionAddress("glDrawBuffers");
    this->_gl->glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) this->GetFunctionAddress("glStencilOpSeparate");
    this->_gl->glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) this->GetFunctionAddress("glStencilFuncSeparate");
    this->_gl->glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) this->GetFunctionAddress("glStencilMaskSeparate");
    this->_gl->glAttachShader = (PFNGLATTACHSHADERPROC) this->GetFunctionAddress("glAttachShader");
    this->_gl->glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) this->GetFunctionAddress("glBindAttribLocation");
    this->_gl->glCompileShader = (PFNGLCOMPILESHADERPROC) this->GetFunctionAddress("glCompileShader");
    this->_gl->glCreateProgram = (PFNGLCREATEPROGRAMPROC) this->GetFunctionAddress("glCreateProgram");
    this->_gl->glCreateShader = (PFNGLCREATESHADERPROC) this->GetFunctionAddress("glCreateShader");
    this->_gl->glDeleteProgram = (PFNGLDELETEPROGRAMPROC) this->GetFunctionAddress("glDeleteProgram");
    this->_gl->glDeleteShader = (PFNGLDELETESHADERPROC) this->GetFunctionAddress("glDeleteShader");
    this->_gl->glDetachShader = (PFNGLDETACHSHADERPROC) this->GetFunctionAddress("glDetachShader");
    this->_gl->glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) this->GetFunctionAddress("glDisableVertexAttribArray");
    this->_gl->glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) this->GetFunctionAddress("glEnableVertexAttribArray");
    this->_gl->glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) this->GetFunctionAddress("glGetActiveAttrib");
    this->_gl->glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) this->GetFunctionAddress("glGetActiveUniform");
    this->_gl->glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) this->GetFunctionAddress("glGetAttachedShaders");
    this->_gl->glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) this->GetFunctionAddress("glGetAttribLocation");
    this->_gl->glGetProgramiv = (PFNGLGETPROGRAMIVPROC) this->GetFunctionAddress("glGetProgramiv");
    this->_gl->glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) this->GetFunctionAddress("glGetProgramInfoLog");
    this->_gl->glGetShaderiv = (PFNGLGETSHADERIVPROC) this->GetFunctionAddress("glGetShaderiv");
    this->_gl->glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) this->GetFunctionAddress("glGetShaderInfoLog");
    this->_gl->glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) this->GetFunctionAddress("glGetShaderSource");
    this->_gl->glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) this->GetFunctionAddress("glGetUniformLocation");
    this->_gl->glGetUniformfv = (PFNGLGETUNIFORMFVPROC) this->GetFunctionAddress("glGetUniformfv");
    this->_gl->glGetUniformiv = (PFNGLGETUNIFORMIVPROC) this->GetFunctionAddress("glGetUniformiv");
    this->_gl->glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC) this->GetFunctionAddress("glGetVertexAttribdv");
    this->_gl->glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) this->GetFunctionAddress("glGetVertexAttribfv");
    this->_gl->glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) this->GetFunctionAddress("glGetVertexAttribiv");
    this->_gl->glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) this->GetFunctionAddress("glGetVertexAttribPointerv");
    this->_gl->glIsProgram = (PFNGLISPROGRAMPROC) this->GetFunctionAddress("glIsProgram");
    this->_gl->glIsShader = (PFNGLISSHADERPROC) this->GetFunctionAddress("glIsShader");
    this->_gl->glLinkProgram = (PFNGLLINKPROGRAMPROC) this->GetFunctionAddress("glLinkProgram");
    this->_gl->glShaderSource = (PFNGLSHADERSOURCEPROC) this->GetFunctionAddress("glShaderSource");
    this->_gl->glUseProgram = (PFNGLUSEPROGRAMPROC) this->GetFunctionAddress("glUseProgram");
    this->_gl->glUniform1f = (PFNGLUNIFORM1FPROC) this->GetFunctionAddress("glUniform1f");
    this->_gl->glUniform2f = (PFNGLUNIFORM2FPROC) this->GetFunctionAddress("glUniform2f");
    this->_gl->glUniform3f = (PFNGLUNIFORM3FPROC) this->GetFunctionAddress("glUniform3f");
    this->_gl->glUniform4f = (PFNGLUNIFORM4FPROC) this->GetFunctionAddress("glUniform4f");
    this->_gl->glUniform1i = (PFNGLUNIFORM1IPROC) this->GetFunctionAddress("glUniform1i");
    this->_gl->glUniform2i = (PFNGLUNIFORM2IPROC) this->GetFunctionAddress("glUniform2i");
    this->_gl->glUniform3i = (PFNGLUNIFORM3IPROC) this->GetFunctionAddress("glUniform3i");
    this->_gl->glUniform4i = (PFNGLUNIFORM4IPROC) this->GetFunctionAddress("glUniform4i");
    this->_gl->glUniform1fv = (PFNGLUNIFORM1FVPROC) this->GetFunctionAddress("glUniform1fv");
    this->_gl->glUniform2fv = (PFNGLUNIFORM2FVPROC) this->GetFunctionAddress("glUniform2fv");
    this->_gl->glUniform3fv = (PFNGLUNIFORM3FVPROC) this->GetFunctionAddress("glUniform3fv");
    this->_gl->glUniform4fv = (PFNGLUNIFORM4FVPROC) this->GetFunctionAddress("glUniform4fv");
    this->_gl->glUniform1iv = (PFNGLUNIFORM1IVPROC) this->GetFunctionAddress("glUniform1iv");
    this->_gl->glUniform2iv = (PFNGLUNIFORM2IVPROC) this->GetFunctionAddress("glUniform2iv");
    this->_gl->glUniform3iv = (PFNGLUNIFORM3IVPROC) this->GetFunctionAddress("glUniform3iv");
    this->_gl->glUniform4iv = (PFNGLUNIFORM4IVPROC) this->GetFunctionAddress("glUniform4iv");
    this->_gl->glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) this->GetFunctionAddress("glUniformMatrix2fv");
    this->_gl->glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) this->GetFunctionAddress("glUniformMatrix3fv");
    this->_gl->glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) this->GetFunctionAddress("glUniformMatrix4fv");
    this->_gl->glValidateProgram = (PFNGLVALIDATEPROGRAMPROC) this->GetFunctionAddress("glValidateProgram");
    this->_gl->glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC) this->GetFunctionAddress("glVertexAttrib1d");
    this->_gl->glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC) this->GetFunctionAddress("glVertexAttrib1dv");
    this->_gl->glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) this->GetFunctionAddress("glVertexAttrib1f");
    this->_gl->glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) this->GetFunctionAddress("glVertexAttrib1fv");
    this->_gl->glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC) this->GetFunctionAddress("glVertexAttrib1s");
    this->_gl->glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC) this->GetFunctionAddress("glVertexAttrib1sv");
    this->_gl->glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC) this->GetFunctionAddress("glVertexAttrib2d");
    this->_gl->glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC) this->GetFunctionAddress("glVertexAttrib2dv");
    this->_gl->glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) this->GetFunctionAddress("glVertexAttrib2f");
    this->_gl->glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) this->GetFunctionAddress("glVertexAttrib2fv");
    this->_gl->glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC) this->GetFunctionAddress("glVertexAttrib2s");
    this->_gl->glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC) this->GetFunctionAddress("glVertexAttrib2sv");
    this->_gl->glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC) this->GetFunctionAddress("glVertexAttrib3d");
    this->_gl->glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC) this->GetFunctionAddress("glVertexAttrib3dv");
    this->_gl->glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) this->GetFunctionAddress("glVertexAttrib3f");
    this->_gl->glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) this->GetFunctionAddress("glVertexAttrib3fv");
    this->_gl->glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC) this->GetFunctionAddress("glVertexAttrib3s");
    this->_gl->glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC) this->GetFunctionAddress("glVertexAttrib3sv");
    this->_gl->glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC) this->GetFunctionAddress("glVertexAttrib4Nbv");
    this->_gl->glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC) this->GetFunctionAddress("glVertexAttrib4Niv");
    this->_gl->glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC) this->GetFunctionAddress("glVertexAttrib4Nsv");
    this->_gl->glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC) this->GetFunctionAddress("glVertexAttrib4Nub");
    this->_gl->glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC) this->GetFunctionAddress("glVertexAttrib4Nubv");
    this->_gl->glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC) this->GetFunctionAddress("glVertexAttrib4Nuiv");
    this->_gl->glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC) this->GetFunctionAddress("glVertexAttrib4Nusv");
    this->_gl->glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC) this->GetFunctionAddress("glVertexAttrib4bv");
    this->_gl->glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC) this->GetFunctionAddress("glVertexAttrib4d");
    this->_gl->glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC) this->GetFunctionAddress("glVertexAttrib4dv");
    this->_gl->glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) this->GetFunctionAddress("glVertexAttrib4f");
    this->_gl->glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) this->GetFunctionAddress("glVertexAttrib4fv");
    this->_gl->glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC) this->GetFunctionAddress("glVertexAttrib4iv");
    this->_gl->glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC) this->GetFunctionAddress("glVertexAttrib4s");
    this->_gl->glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC) this->GetFunctionAddress("glVertexAttrib4sv");
    this->_gl->glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC) this->GetFunctionAddress("glVertexAttrib4ubv");
    this->_gl->glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC) this->GetFunctionAddress("glVertexAttrib4uiv");
    this->_gl->glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC) this->GetFunctionAddress("glVertexAttrib4usv");
    this->_gl->glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) this->GetFunctionAddress("glVertexAttribPointer");
    this->_gl->glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) this->GetFunctionAddress("glUniformMatrix2x3fv");
    this->_gl->glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) this->GetFunctionAddress("glUniformMatrix3x2fv");
    this->_gl->glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) this->GetFunctionAddress("glUniformMatrix2x4fv");
    this->_gl->glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) this->GetFunctionAddress("glUniformMatrix4x2fv");
    this->_gl->glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) this->GetFunctionAddress("glUniformMatrix3x4fv");
    this->_gl->glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) this->GetFunctionAddress("glUniformMatrix4x3fv");
    this->_gl->glColorMaski = (PFNGLCOLORMASKIPROC) this->GetFunctionAddress("glColorMaski");
    this->_gl->glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC) this->GetFunctionAddress("glGetBooleani_v");
    this->_gl->glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) this->GetFunctionAddress("glGetIntegeri_v");
    this->_gl->glEnablei = (PFNGLENABLEIPROC) this->GetFunctionAddress("glEnablei");
    this->_gl->glDisablei = (PFNGLDISABLEIPROC) this->GetFunctionAddress("glDisablei");
    this->_gl->glIsEnabledi = (PFNGLISENABLEDIPROC) this->GetFunctionAddress("glIsEnabledi");
    this->_gl->glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) this->GetFunctionAddress("glBeginTransformFeedback");
    this->_gl->glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) this->GetFunctionAddress("glEndTransformFeedback");
    this->_gl->glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) this->GetFunctionAddress("glBindBufferRange");
    this->_gl->glBindBufferBase = (PFNGLBINDBUFFERBASEPROC) this->GetFunctionAddress("glBindBufferBase");
    this->_gl->glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) this->GetFunctionAddress("glTransformFeedbackVaryings");
    this->_gl->glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) this->GetFunctionAddress("glGetTransformFeedbackVarying");
    this->_gl->glClampColor = (PFNGLCLAMPCOLORPROC) this->GetFunctionAddress("glClampColor");
    this->_gl->glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC) this->GetFunctionAddress("glBeginConditionalRender");
    this->_gl->glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC) this->GetFunctionAddress("glEndConditionalRender");
    this->_gl->glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) this->GetFunctionAddress("glVertexAttribIPointer");
    this->_gl->glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) this->GetFunctionAddress("glGetVertexAttribIiv");
    this->_gl->glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) this->GetFunctionAddress("glGetVertexAttribIuiv");
    this->_gl->glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC) this->GetFunctionAddress("glVertexAttribI1i");
    this->_gl->glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC) this->GetFunctionAddress("glVertexAttribI2i");
    this->_gl->glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC) this->GetFunctionAddress("glVertexAttribI3i");
    this->_gl->glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC) this->GetFunctionAddress("glVertexAttribI4i");
    this->_gl->glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC) this->GetFunctionAddress("glVertexAttribI1ui");
    this->_gl->glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC) this->GetFunctionAddress("glVertexAttribI2ui");
    this->_gl->glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC) this->GetFunctionAddress("glVertexAttribI3ui");
    this->_gl->glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC) this->GetFunctionAddress("glVertexAttribI4ui");
    this->_gl->glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC) this->GetFunctionAddress("glVertexAttribI1iv");
    this->_gl->glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC) this->GetFunctionAddress("glVertexAttribI2iv");
    this->_gl->glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC) this->GetFunctionAddress("glVertexAttribI3iv");
    this->_gl->glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC) this->GetFunctionAddress("glVertexAttribI4iv");
    this->_gl->glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC) this->GetFunctionAddress("glVertexAttribI1uiv");
    this->_gl->glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC) this->GetFunctionAddress("glVertexAttribI2uiv");
    this->_gl->glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC) this->GetFunctionAddress("glVertexAttribI3uiv");
    this->_gl->glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC) this->GetFunctionAddress("glVertexAttribI4uiv");
    this->_gl->glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC) this->GetFunctionAddress("glVertexAttribI4bv");
    this->_gl->glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC) this->GetFunctionAddress("glVertexAttribI4sv");
    this->_gl->glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC) this->GetFunctionAddress("glVertexAttribI4ubv");
    this->_gl->glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC) this->GetFunctionAddress("glVertexAttribI4usv");
    this->_gl->glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC) this->GetFunctionAddress("glGetUniformuiv");
    this->_gl->glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC) this->GetFunctionAddress("glBindFragDataLocation");
    this->_gl->glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) this->GetFunctionAddress("glGetFragDataLocation");
    this->_gl->glUniform1ui = (PFNGLUNIFORM1UIPROC) this->GetFunctionAddress("glUniform1ui");
    this->_gl->glUniform2ui = (PFNGLUNIFORM2UIPROC) this->GetFunctionAddress("glUniform2ui");
    this->_gl->glUniform3ui = (PFNGLUNIFORM3UIPROC) this->GetFunctionAddress("glUniform3ui");
    this->_gl->glUniform4ui = (PFNGLUNIFORM4UIPROC) this->GetFunctionAddress("glUniform4ui");
    this->_gl->glUniform1uiv = (PFNGLUNIFORM1UIVPROC) this->GetFunctionAddress("glUniform1uiv");
    this->_gl->glUniform2uiv = (PFNGLUNIFORM2UIVPROC) this->GetFunctionAddress("glUniform2uiv");
    this->_gl->glUniform3uiv = (PFNGLUNIFORM3UIVPROC) this->GetFunctionAddress("glUniform3uiv");
    this->_gl->glUniform4uiv = (PFNGLUNIFORM4UIVPROC) this->GetFunctionAddress("glUniform4uiv");
    this->_gl->glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC) this->GetFunctionAddress("glTexParameterIiv");
    this->_gl->glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC) this->GetFunctionAddress("glTexParameterIuiv");
    this->_gl->glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC) this->GetFunctionAddress("glGetTexParameterIiv");
    this->_gl->glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC) this->GetFunctionAddress("glGetTexParameterIuiv");
    this->_gl->glClearBufferiv = (PFNGLCLEARBUFFERIVPROC) this->GetFunctionAddress("glClearBufferiv");
    this->_gl->glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) this->GetFunctionAddress("glClearBufferuiv");
    this->_gl->glClearBufferfv = (PFNGLCLEARBUFFERFVPROC) this->GetFunctionAddress("glClearBufferfv");
    this->_gl->glClearBufferfi = (PFNGLCLEARBUFFERFIPROC) this->GetFunctionAddress("glClearBufferfi");
    this->_gl->glGetStringi = (PFNGLGETSTRINGIPROC) this->GetFunctionAddress("glGetStringi");
    this->_gl->glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) this->GetFunctionAddress("glIsRenderbuffer");
    this->_gl->glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) this->GetFunctionAddress("glBindRenderbuffer");
    this->_gl->glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) this->GetFunctionAddress("glDeleteRenderbuffers");
    this->_gl->glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) this->GetFunctionAddress("glGenRenderbuffers");
    this->_gl->glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) this->GetFunctionAddress("glRenderbufferStorage");
    this->_gl->glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) this->GetFunctionAddress("glGetRenderbufferParameteriv");
    this->_gl->glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) this->GetFunctionAddress("glIsFramebuffer");
    this->_gl->glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) this->GetFunctionAddress("glBindFramebuffer");
    this->_gl->glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) this->GetFunctionAddress("glDeleteFramebuffers");
    this->_gl->glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) this->GetFunctionAddress("glGenFramebuffers");
    this->_gl->glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) this->GetFunctionAddress("glCheckFramebufferStatus");
    this->_gl->glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) this->GetFunctionAddress("glFramebufferTexture1D");
    this->_gl->glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) this->GetFunctionAddress("glFramebufferTexture2D");
    this->_gl->glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) this->GetFunctionAddress("glFramebufferTexture3D");
    this->_gl->glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) this->GetFunctionAddress("glFramebufferRenderbuffer");
    this->_gl->glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) this->GetFunctionAddress("glGetFramebufferAttachmentParameteriv");
    this->_gl->glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) this->GetFunctionAddress("glGenerateMipmap");
    this->_gl->glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) this->GetFunctionAddress("glBlitFramebuffer");
    this->_gl->glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) this->GetFunctionAddress("glRenderbufferStorageMultisample");
    this->_gl->glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) this->GetFunctionAddress("glFramebufferTextureLayer");
    this->_gl->glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) this->GetFunctionAddress("glMapBufferRange");
    this->_gl->glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) this->GetFunctionAddress("glFlushMappedBufferRange");
    this->_gl->glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) this->GetFunctionAddress("glBindVertexArray");
    this->_gl->glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) this->GetFunctionAddress("glDeleteVertexArrays");
    this->_gl->glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) this->GetFunctionAddress("glGenVertexArrays");
    this->_gl->glIsVertexArray = (PFNGLISVERTEXARRAYPROC) this->GetFunctionAddress("glIsVertexArray");
    this->_gl->glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC) this->GetFunctionAddress("glDrawArraysInstanced");
    this->_gl->glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) this->GetFunctionAddress("glDrawElementsInstanced");
    this->_gl->glTexBuffer = (PFNGLTEXBUFFERPROC) this->GetFunctionAddress("glTexBuffer");
    this->_gl->glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC) this->GetFunctionAddress("glPrimitiveRestartIndex");
    this->_gl->glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC) this->GetFunctionAddress("glCopyBufferSubData");
    this->_gl->glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC) this->GetFunctionAddress("glGetUniformIndices");
    this->_gl->glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC) this->GetFunctionAddress("glGetActiveUniformsiv");
    this->_gl->glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC) this->GetFunctionAddress("glGetActiveUniformName");
    this->_gl->glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC) this->GetFunctionAddress("glGetUniformBlockIndex");
    this->_gl->glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) this->GetFunctionAddress("glGetActiveUniformBlockiv");
    this->_gl->glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) this->GetFunctionAddress("glGetActiveUniformBlockName");
    this->_gl->glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC) this->GetFunctionAddress("glUniformBlockBinding");
    this->_gl->glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC) this->GetFunctionAddress("glDrawElementsBaseVertex");
    this->_gl->glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC) this->GetFunctionAddress("glDrawRangeElementsBaseVertex");
    this->_gl->glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC) this->GetFunctionAddress("glDrawElementsInstancedBaseVertex");
    this->_gl->glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC) this->GetFunctionAddress("glMultiDrawElementsBaseVertex");
    this->_gl->glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC) this->GetFunctionAddress("glProvokingVertex");
    this->_gl->glFenceSync = (PFNGLFENCESYNCPROC) this->GetFunctionAddress("glFenceSync");
    this->_gl->glIsSync = (PFNGLISSYNCPROC) this->GetFunctionAddress("glIsSync");
    this->_gl->glDeleteSync = (PFNGLDELETESYNCPROC) this->GetFunctionAddress("glDeleteSync");
    this->_gl->glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC) this->GetFunctionAddress("glClientWaitSync");
    this->_gl->glWaitSync = (PFNGLWAITSYNCPROC) this->GetFunctionAddress("glWaitSync");
    this->_gl->glGetInteger64v = (PFNGLGETINTEGER64VPROC) this->GetFunctionAddress("glGetInteger64v");
    this->_gl->glGetSynciv = (PFNGLGETSYNCIVPROC) this->GetFunctionAddress("glGetSynciv");
    this->_gl->glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) this->GetFunctionAddress("glGetInteger64i_v");
    this->_gl->glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) this->GetFunctionAddress("glGetBufferParameteri64v");
    this->_gl->glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) this->GetFunctionAddress("glFramebufferTexture");
    this->_gl->glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) this->GetFunctionAddress("glTexImage2DMultisample");
    this->_gl->glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) this->GetFunctionAddress("glTexImage3DMultisample");
    this->_gl->glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) this->GetFunctionAddress("glGetMultisamplefv");
    this->_gl->glSampleMaski = (PFNGLSAMPLEMASKIPROC) this->GetFunctionAddress("glSampleMaski");
    this->_gl->glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC) this->GetFunctionAddress("glBindFragDataLocationIndexed");
    this->_gl->glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC) this->GetFunctionAddress("glGetFragDataIndex");
    this->_gl->glGenSamplers = (PFNGLGENSAMPLERSPROC) this->GetFunctionAddress("glGenSamplers");
    this->_gl->glDeleteSamplers = (PFNGLDELETESAMPLERSPROC) this->GetFunctionAddress("glDeleteSamplers");
    this->_gl->glIsSampler = (PFNGLISSAMPLERPROC) this->GetFunctionAddress("glIsSampler");
    this->_gl->glBindSampler = (PFNGLBINDSAMPLERPROC) this->GetFunctionAddress("glBindSampler");
    this->_gl->glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) this->GetFunctionAddress("glSamplerParameteri");
    this->_gl->glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) this->GetFunctionAddress("glSamplerParameteriv");
    this->_gl->glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) this->GetFunctionAddress("glSamplerParameterf");
    this->_gl->glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) this->GetFunctionAddress("glSamplerParameterfv");
    this->_gl->glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC) this->GetFunctionAddress("glSamplerParameterIiv");
    this->_gl->glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC) this->GetFunctionAddress("glSamplerParameterIuiv");
    this->_gl->glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) this->GetFunctionAddress("glGetSamplerParameteriv");
    this->_gl->glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC) this->GetFunctionAddress("glGetSamplerParameterIiv");
    this->_gl->glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) this->GetFunctionAddress("glGetSamplerParameterfv");
    this->_gl->glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC) this->GetFunctionAddress("glGetSamplerParameterIuiv");
    this->_gl->glQueryCounter = (PFNGLQUERYCOUNTERPROC) this->GetFunctionAddress("glQueryCounter");
    this->_gl->glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC) this->GetFunctionAddress("glGetQueryObjecti64v");
    this->_gl->glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC) this->GetFunctionAddress("glGetQueryObjectui64v");
    this->_gl->glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC) this->GetFunctionAddress("glVertexAttribDivisor");
    this->_gl->glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC) this->GetFunctionAddress("glVertexAttribP1ui");
    this->_gl->glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC) this->GetFunctionAddress("glVertexAttribP1uiv");
    this->_gl->glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC) this->GetFunctionAddress("glVertexAttribP2ui");
    this->_gl->glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC) this->GetFunctionAddress("glVertexAttribP2uiv");
    this->_gl->glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC) this->GetFunctionAddress("glVertexAttribP3ui");
    this->_gl->glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC) this->GetFunctionAddress("glVertexAttribP3uiv");
    this->_gl->glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC) this->GetFunctionAddress("glVertexAttribP4ui");
    this->_gl->glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC) this->GetFunctionAddress("glVertexAttribP4uiv");
    this->_gl->glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC) this->GetFunctionAddress("glMinSampleShading");
    this->_gl->glBlendEquationi = (PFNGLBLENDEQUATIONIPROC) this->GetFunctionAddress("glBlendEquationi");
    this->_gl->glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC) this->GetFunctionAddress("glBlendEquationSeparatei");
    this->_gl->glBlendFunci = (PFNGLBLENDFUNCIPROC) this->GetFunctionAddress("glBlendFunci");
    this->_gl->glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC) this->GetFunctionAddress("glBlendFuncSeparatei");
    this->_gl->glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC) this->GetFunctionAddress("glDrawArraysIndirect");
    this->_gl->glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC) this->GetFunctionAddress("glDrawElementsIndirect");
    this->_gl->glUniform1d = (PFNGLUNIFORM1DPROC) this->GetFunctionAddress("glUniform1d");
    this->_gl->glUniform2d = (PFNGLUNIFORM2DPROC) this->GetFunctionAddress("glUniform2d");
    this->_gl->glUniform3d = (PFNGLUNIFORM3DPROC) this->GetFunctionAddress("glUniform3d");
    this->_gl->glUniform4d = (PFNGLUNIFORM4DPROC) this->GetFunctionAddress("glUniform4d");
    this->_gl->glUniform1dv = (PFNGLUNIFORM1DVPROC) this->GetFunctionAddress("glUniform1dv");
    this->_gl->glUniform2dv = (PFNGLUNIFORM2DVPROC) this->GetFunctionAddress("glUniform2dv");
    this->_gl->glUniform3dv = (PFNGLUNIFORM3DVPROC) this->GetFunctionAddress("glUniform3dv");
    this->_gl->glUniform4dv = (PFNGLUNIFORM4DVPROC) this->GetFunctionAddress("glUniform4dv");
    this->_gl->glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC) this->GetFunctionAddress("glUniformMatrix2dv");
    this->_gl->glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC) this->GetFunctionAddress("glUniformMatrix3dv");
    this->_gl->glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC) this->GetFunctionAddress("glUniformMatrix4dv");
    this->_gl->glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC) this->GetFunctionAddress("glUniformMatrix2x3dv");
    this->_gl->glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC) this->GetFunctionAddress("glUniformMatrix2x4dv");
    this->_gl->glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC) this->GetFunctionAddress("glUniformMatrix3x2dv");
    this->_gl->glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC) this->GetFunctionAddress("glUniformMatrix3x4dv");
    this->_gl->glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC) this->GetFunctionAddress("glUniformMatrix4x2dv");
    this->_gl->glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC) this->GetFunctionAddress("glUniformMatrix4x3dv");
    this->_gl->glGetUniformdv = (PFNGLGETUNIFORMDVPROC) this->GetFunctionAddress("glGetUniformdv");
    this->_gl->glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC) this->GetFunctionAddress("glGetSubroutineUniformLocation");
    this->_gl->glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC) this->GetFunctionAddress("glGetSubroutineIndex");
    this->_gl->glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC) this->GetFunctionAddress("glGetActiveSubroutineUniformiv");
    this->_gl->glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC) this->GetFunctionAddress("glGetActiveSubroutineUniformName");
    this->_gl->glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC) this->GetFunctionAddress("glGetActiveSubroutineName");
    this->_gl->glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC) this->GetFunctionAddress("glUniformSubroutinesuiv");
    this->_gl->glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC) this->GetFunctionAddress("glGetUniformSubroutineuiv");
    this->_gl->glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC) this->GetFunctionAddress("glGetProgramStageiv");
    this->_gl->glPatchParameteri = (PFNGLPATCHPARAMETERIPROC) this->GetFunctionAddress("glPatchParameteri");
    this->_gl->glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC) this->GetFunctionAddress("glPatchParameterfv");
    this->_gl->glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC) this->GetFunctionAddress("glBindTransformFeedback");
    this->_gl->glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC) this->GetFunctionAddress("glDeleteTransformFeedbacks");
    this->_gl->glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC) this->GetFunctionAddress("glGenTransformFeedbacks");
    this->_gl->glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC) this->GetFunctionAddress("glIsTransformFeedback");
    this->_gl->glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC) this->GetFunctionAddress("glPauseTransformFeedback");
    this->_gl->glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC) this->GetFunctionAddress("glResumeTransformFeedback");
    this->_gl->glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC) this->GetFunctionAddress("glDrawTransformFeedback");
    this->_gl->glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC) this->GetFunctionAddress("glDrawTransformFeedbackStream");
    this->_gl->glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC) this->GetFunctionAddress("glBeginQueryIndexed");
    this->_gl->glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC) this->GetFunctionAddress("glEndQueryIndexed");
    this->_gl->glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC) this->GetFunctionAddress("glGetQueryIndexediv");
    this->_gl->glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC) this->GetFunctionAddress("glReleaseShaderCompiler");
    this->_gl->glShaderBinary = (PFNGLSHADERBINARYPROC) this->GetFunctionAddress("glShaderBinary");
    this->_gl->glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC) this->GetFunctionAddress("glGetShaderPrecisionFormat");
    this->_gl->glDepthRangef = (PFNGLDEPTHRANGEFPROC) this->GetFunctionAddress("glDepthRangef");
    this->_gl->glClearDepthf = (PFNGLCLEARDEPTHFPROC) this->GetFunctionAddress("glClearDepthf");
    this->_gl->glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC) this->GetFunctionAddress("glGetProgramBinary");
    this->_gl->glProgramBinary = (PFNGLPROGRAMBINARYPROC) this->GetFunctionAddress("glProgramBinary");
    this->_gl->glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) this->GetFunctionAddress("glProgramParameteri");
    this->_gl->glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC) this->GetFunctionAddress("glUseProgramStages");
    this->_gl->glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC) this->GetFunctionAddress("glActiveShaderProgram");
    this->_gl->glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC) this->GetFunctionAddress("glCreateShaderProgramv");
    this->_gl->glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC) this->GetFunctionAddress("glBindProgramPipeline");
    this->_gl->glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC) this->GetFunctionAddress("glDeleteProgramPipelines");
    this->_gl->glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC) this->GetFunctionAddress("glGenProgramPipelines");
    this->_gl->glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC) this->GetFunctionAddress("glIsProgramPipeline");
    this->_gl->glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC) this->GetFunctionAddress("glGetProgramPipelineiv");
    this->_gl->glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC) this->GetFunctionAddress("glProgramUniform1i");
    this->_gl->glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC) this->GetFunctionAddress("glProgramUniform1iv");
    this->_gl->glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC) this->GetFunctionAddress("glProgramUniform1f");
    this->_gl->glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC) this->GetFunctionAddress("glProgramUniform1fv");
    this->_gl->glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC) this->GetFunctionAddress("glProgramUniform1d");
    this->_gl->glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC) this->GetFunctionAddress("glProgramUniform1dv");
    this->_gl->glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC) this->GetFunctionAddress("glProgramUniform1ui");
    this->_gl->glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC) this->GetFunctionAddress("glProgramUniform1uiv");
    this->_gl->glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC) this->GetFunctionAddress("glProgramUniform2i");
    this->_gl->glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC) this->GetFunctionAddress("glProgramUniform2iv");
    this->_gl->glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC) this->GetFunctionAddress("glProgramUniform2f");
    this->_gl->glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC) this->GetFunctionAddress("glProgramUniform2fv");
    this->_gl->glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC) this->GetFunctionAddress("glProgramUniform2d");
    this->_gl->glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC) this->GetFunctionAddress("glProgramUniform2dv");
    this->_gl->glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC) this->GetFunctionAddress("glProgramUniform2ui");
    this->_gl->glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC) this->GetFunctionAddress("glProgramUniform2uiv");
    this->_gl->glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC) this->GetFunctionAddress("glProgramUniform3i");
    this->_gl->glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC) this->GetFunctionAddress("glProgramUniform3iv");
    this->_gl->glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC) this->GetFunctionAddress("glProgramUniform3f");
    this->_gl->glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC) this->GetFunctionAddress("glProgramUniform3fv");
    this->_gl->glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC) this->GetFunctionAddress("glProgramUniform3d");
    this->_gl->glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC) this->GetFunctionAddress("glProgramUniform3dv");
    this->_gl->glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC) this->GetFunctionAddress("glProgramUniform3ui");
    this->_gl->glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC) this->GetFunctionAddress("glProgramUniform3uiv");
    this->_gl->glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC) this->GetFunctionAddress("glProgramUniform4i");
    this->_gl->glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC) this->GetFunctionAddress("glProgramUniform4iv");
    this->_gl->glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC) this->GetFunctionAddress("glProgramUniform4f");
    this->_gl->glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC) this->GetFunctionAddress("glProgramUniform4fv");
    this->_gl->glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC) this->GetFunctionAddress("glProgramUniform4d");
    this->_gl->glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC) this->GetFunctionAddress("glProgramUniform4dv");
    this->_gl->glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC) this->GetFunctionAddress("glProgramUniform4ui");
    this->_gl->glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC) this->GetFunctionAddress("glProgramUniform4uiv");
    this->_gl->glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC) this->GetFunctionAddress("glProgramUniformMatrix2fv");
    this->_gl->glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC) this->GetFunctionAddress("glProgramUniformMatrix3fv");
    this->_gl->glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC) this->GetFunctionAddress("glProgramUniformMatrix4fv");
    this->_gl->glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC) this->GetFunctionAddress("glProgramUniformMatrix2dv");
    this->_gl->glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC) this->GetFunctionAddress("glProgramUniformMatrix3dv");
    this->_gl->glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC) this->GetFunctionAddress("glProgramUniformMatrix4dv");
    this->_gl->glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC) this->GetFunctionAddress("glProgramUniformMatrix2x3fv");
    this->_gl->glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC) this->GetFunctionAddress("glProgramUniformMatrix3x2fv");
    this->_gl->glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC) this->GetFunctionAddress("glProgramUniformMatrix2x4fv");
    this->_gl->glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC) this->GetFunctionAddress("glProgramUniformMatrix4x2fv");
    this->_gl->glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC) this->GetFunctionAddress("glProgramUniformMatrix3x4fv");
    this->_gl->glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC) this->GetFunctionAddress("glProgramUniformMatrix4x3fv");
    this->_gl->glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC) this->GetFunctionAddress("glProgramUniformMatrix2x3dv");
    this->_gl->glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC) this->GetFunctionAddress("glProgramUniformMatrix3x2dv");
    this->_gl->glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC) this->GetFunctionAddress("glProgramUniformMatrix2x4dv");
    this->_gl->glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC) this->GetFunctionAddress("glProgramUniformMatrix4x2dv");
    this->_gl->glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC) this->GetFunctionAddress("glProgramUniformMatrix3x4dv");
    this->_gl->glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC) this->GetFunctionAddress("glProgramUniformMatrix4x3dv");
    this->_gl->glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC) this->GetFunctionAddress("glValidateProgramPipeline");
    this->_gl->glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC) this->GetFunctionAddress("glGetProgramPipelineInfoLog");
    this->_gl->glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC) this->GetFunctionAddress("glVertexAttribL1d");
    this->_gl->glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC) this->GetFunctionAddress("glVertexAttribL2d");
    this->_gl->glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC) this->GetFunctionAddress("glVertexAttribL3d");
    this->_gl->glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC) this->GetFunctionAddress("glVertexAttribL4d");
    this->_gl->glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC) this->GetFunctionAddress("glVertexAttribL1dv");
    this->_gl->glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC) this->GetFunctionAddress("glVertexAttribL2dv");
    this->_gl->glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC) this->GetFunctionAddress("glVertexAttribL3dv");
    this->_gl->glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC) this->GetFunctionAddress("glVertexAttribL4dv");
    this->_gl->glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC) this->GetFunctionAddress("glVertexAttribLPointer");
    this->_gl->glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC) this->GetFunctionAddress("glGetVertexAttribLdv");
    this->_gl->glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC) this->GetFunctionAddress("glViewportArrayv");
    this->_gl->glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC) this->GetFunctionAddress("glViewportIndexedf");
    this->_gl->glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC) this->GetFunctionAddress("glViewportIndexedfv");
    this->_gl->glScissorArrayv = (PFNGLSCISSORARRAYVPROC) this->GetFunctionAddress("glScissorArrayv");
    this->_gl->glScissorIndexed = (PFNGLSCISSORINDEXEDPROC) this->GetFunctionAddress("glScissorIndexed");
    this->_gl->glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC) this->GetFunctionAddress("glScissorIndexedv");
    this->_gl->glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC) this->GetFunctionAddress("glDepthRangeArrayv");
    this->_gl->glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC) this->GetFunctionAddress("glDepthRangeIndexed");
    this->_gl->glGetFloati_v = (PFNGLGETFLOATI_VPROC) this->GetFunctionAddress("glGetFloati_v");
    this->_gl->glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC) this->GetFunctionAddress("glGetDoublei_v");
    this->_gl->glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC) this->GetFunctionAddress("glDrawArraysInstancedBaseInstance");
    this->_gl->glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC) this->GetFunctionAddress("glDrawElementsInstancedBaseInstance");
    this->_gl->glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC) this->GetFunctionAddress("glDrawElementsInstancedBaseVertexBaseInstance");
    this->_gl->glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC) this->GetFunctionAddress("glGetInternalformativ");
    this->_gl->glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC) this->GetFunctionAddress("glGetActiveAtomicCounterBufferiv");
    this->_gl->glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC) this->GetFunctionAddress("glBindImageTexture");
    this->_gl->glMemoryBarrier = (PFNGLMEMORYBARRIERPROC) this->GetFunctionAddress("glMemoryBarrier");
    this->_gl->glTexStorage1D = (PFNGLTEXSTORAGE1DPROC) this->GetFunctionAddress("glTexStorage1D");
    this->_gl->glTexStorage2D = (PFNGLTEXSTORAGE2DPROC) this->GetFunctionAddress("glTexStorage2D");
    this->_gl->glTexStorage3D = (PFNGLTEXSTORAGE3DPROC) this->GetFunctionAddress("glTexStorage3D");
    this->_gl->glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC) this->GetFunctionAddress("glDrawTransformFeedbackInstanced");
    this->_gl->glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC) this->GetFunctionAddress("glDrawTransformFeedbackStreamInstanced");
    this->_gl->glClearBufferData = (PFNGLCLEARBUFFERDATAPROC) this->GetFunctionAddress("glClearBufferData");
    this->_gl->glClearBufferSubData = (PFNGLCLEARBUFFERSUBDATAPROC) this->GetFunctionAddress("glClearBufferSubData");
    this->_gl->glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC) this->GetFunctionAddress("glDispatchCompute");
    this->_gl->glDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC) this->GetFunctionAddress("glDispatchComputeIndirect");
    this->_gl->glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC) this->GetFunctionAddress("glCopyImageSubData");
    this->_gl->glFramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC) this->GetFunctionAddress("glFramebufferParameteri");
    this->_gl->glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC) this->GetFunctionAddress("glGetFramebufferParameteriv");
    this->_gl->glGetInternalformati64v = (PFNGLGETINTERNALFORMATI64VPROC) this->GetFunctionAddress("glGetInternalformati64v");
    this->_gl->glInvalidateTexSubImage = (PFNGLINVALIDATETEXSUBIMAGEPROC) this->GetFunctionAddress("glInvalidateTexSubImage");
    this->_gl->glInvalidateTexImage = (PFNGLINVALIDATETEXIMAGEPROC) this->GetFunctionAddress("glInvalidateTexImage");
    this->_gl->glInvalidateBufferSubData = (PFNGLINVALIDATEBUFFERSUBDATAPROC) this->GetFunctionAddress("glInvalidateBufferSubData");
    this->_gl->glInvalidateBufferData = (PFNGLINVALIDATEBUFFERDATAPROC) this->GetFunctionAddress("glInvalidateBufferData");
    this->_gl->glInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC) this->GetFunctionAddress("glInvalidateFramebuffer");
    this->_gl->glInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC) this->GetFunctionAddress("glInvalidateSubFramebuffer");
    this->_gl->glMultiDrawArraysIndirect = (PFNGLMULTIDRAWARRAYSINDIRECTPROC) this->GetFunctionAddress("glMultiDrawArraysIndirect");
    this->_gl->glMultiDrawElementsIndirect = (PFNGLMULTIDRAWELEMENTSINDIRECTPROC) this->GetFunctionAddress("glMultiDrawElementsIndirect");
    this->_gl->glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC) this->GetFunctionAddress("glGetProgramInterfaceiv");
    this->_gl->glGetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC) this->GetFunctionAddress("glGetProgramResourceIndex");
    this->_gl->glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC) this->GetFunctionAddress("glGetProgramResourceName");
    this->_gl->glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC) this->GetFunctionAddress("glGetProgramResourceiv");
    this->_gl->glGetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC) this->GetFunctionAddress("glGetProgramResourceLocation");
    this->_gl->glGetProgramResourceLocationIndex = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC) this->GetFunctionAddress("glGetProgramResourceLocationIndex");
    this->_gl->glShaderStorageBlockBinding = (PFNGLSHADERSTORAGEBLOCKBINDINGPROC) this->GetFunctionAddress("glShaderStorageBlockBinding");
    this->_gl->glTexBufferRange = (PFNGLTEXBUFFERRANGEPROC) this->GetFunctionAddress("glTexBufferRange");
    this->_gl->glTexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC) this->GetFunctionAddress("glTexStorage2DMultisample");
    this->_gl->glTexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC) this->GetFunctionAddress("glTexStorage3DMultisample");
    this->_gl->glTextureView = (PFNGLTEXTUREVIEWPROC) this->GetFunctionAddress("glTextureView");
    this->_gl->glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC) this->GetFunctionAddress("glBindVertexBuffer");
    this->_gl->glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC) this->GetFunctionAddress("glVertexAttribFormat");
    this->_gl->glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC) this->GetFunctionAddress("glVertexAttribIFormat");
    this->_gl->glVertexAttribLFormat = (PFNGLVERTEXATTRIBLFORMATPROC) this->GetFunctionAddress("glVertexAttribLFormat");
    this->_gl->glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC) this->GetFunctionAddress("glVertexAttribBinding");
    this->_gl->glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC) this->GetFunctionAddress("glVertexBindingDivisor");
    this->_gl->glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC) this->GetFunctionAddress("glDebugMessageControl");
    this->_gl->glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC) this->GetFunctionAddress("glDebugMessageInsert");
    this->_gl->glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC) this->GetFunctionAddress("glDebugMessageCallback");
    this->_gl->glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC) this->GetFunctionAddress("glGetDebugMessageLog");
    this->_gl->glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC) this->GetFunctionAddress("glPushDebugGroup");
    this->_gl->glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC) this->GetFunctionAddress("glPopDebugGroup");
    this->_gl->glObjectLabel = (PFNGLOBJECTLABELPROC) this->GetFunctionAddress("glObjectLabel");
    this->_gl->glGetObjectLabel = (PFNGLGETOBJECTLABELPROC) this->GetFunctionAddress("glGetObjectLabel");
    this->_gl->glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC) this->GetFunctionAddress("glObjectPtrLabel");
    this->_gl->glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC) this->GetFunctionAddress("glGetObjectPtrLabel");
    this->_gl->glBufferStorage = (PFNGLBUFFERSTORAGEPROC) this->GetFunctionAddress("glBufferStorage");
    this->_gl->glClearTexImage = (PFNGLCLEARTEXIMAGEPROC) this->GetFunctionAddress("glClearTexImage");
    this->_gl->glClearTexSubImage = (PFNGLCLEARTEXSUBIMAGEPROC) this->GetFunctionAddress("glClearTexSubImage");
    this->_gl->glBindBuffersBase = (PFNGLBINDBUFFERSBASEPROC) this->GetFunctionAddress("glBindBuffersBase");
    this->_gl->glBindBuffersRange = (PFNGLBINDBUFFERSRANGEPROC) this->GetFunctionAddress("glBindBuffersRange");
    this->_gl->glBindTextures = (PFNGLBINDTEXTURESPROC) this->GetFunctionAddress("glBindTextures");
    this->_gl->glBindSamplers = (PFNGLBINDSAMPLERSPROC) this->GetFunctionAddress("glBindSamplers");
    this->_gl->glBindImageTextures = (PFNGLBINDIMAGETEXTURESPROC) this->GetFunctionAddress("glBindImageTextures");
    this->_gl->glBindVertexBuffers = (PFNGLBINDVERTEXBUFFERSPROC) this->GetFunctionAddress("glBindVertexBuffers");
    this->_gl->glClipControl = (PFNGLCLIPCONTROLPROC) this->GetFunctionAddress("glClipControl");
    this->_gl->glCreateTransformFeedbacks = (PFNGLCREATETRANSFORMFEEDBACKSPROC) this->GetFunctionAddress("glCreateTransformFeedbacks");
    this->_gl->glTransformFeedbackBufferBase = (PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC) this->GetFunctionAddress("glTransformFeedbackBufferBase");
    this->_gl->glTransformFeedbackBufferRange = (PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC) this->GetFunctionAddress("glTransformFeedbackBufferRange");
    this->_gl->glGetTransformFeedbackiv = (PFNGLGETTRANSFORMFEEDBACKIVPROC) this->GetFunctionAddress("glGetTransformFeedbackiv");
    this->_gl->glGetTransformFeedbacki_v = (PFNGLGETTRANSFORMFEEDBACKI_VPROC) this->GetFunctionAddress("glGetTransformFeedbacki_v");
    this->_gl->glGetTransformFeedbacki64_v = (PFNGLGETTRANSFORMFEEDBACKI64_VPROC) this->GetFunctionAddress("glGetTransformFeedbacki64_v");
    this->_gl->glCreateBuffers = (PFNGLCREATEBUFFERSPROC) this->GetFunctionAddress("glCreateBuffers");
    this->_gl->glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC) this->GetFunctionAddress("glNamedBufferStorage");
    this->_gl->glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC) this->GetFunctionAddress("glNamedBufferData");
    this->_gl->glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC) this->GetFunctionAddress("glNamedBufferSubData");
    this->_gl->glCopyNamedBufferSubData = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC) this->GetFunctionAddress("glCopyNamedBufferSubData");
    this->_gl->glClearNamedBufferData = (PFNGLCLEARNAMEDBUFFERDATAPROC) this->GetFunctionAddress("glClearNamedBufferData");
    this->_gl->glClearNamedBufferSubData = (PFNGLCLEARNAMEDBUFFERSUBDATAPROC) this->GetFunctionAddress("glClearNamedBufferSubData");
    this->_gl->glMapNamedBuffer = (PFNGLMAPNAMEDBUFFERPROC) this->GetFunctionAddress("glMapNamedBuffer");
    this->_gl->glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC) this->GetFunctionAddress("glMapNamedBufferRange");
    this->_gl->glUnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC) this->GetFunctionAddress("glUnmapNamedBuffer");
    this->_gl->glFlushMappedNamedBufferRange = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC) this->GetFunctionAddress("glFlushMappedNamedBufferRange");
    this->_gl->glGetNamedBufferParameteriv = (PFNGLGETNAMEDBUFFERPARAMETERIVPROC) this->GetFunctionAddress("glGetNamedBufferParameteriv");
    this->_gl->glGetNamedBufferParameteri64v = (PFNGLGETNAMEDBUFFERPARAMETERI64VPROC) this->GetFunctionAddress("glGetNamedBufferParameteri64v");
    this->_gl->glGetNamedBufferPointerv = (PFNGLGETNAMEDBUFFERPOINTERVPROC) this->GetFunctionAddress("glGetNamedBufferPointerv");
    this->_gl->glGetNamedBufferSubData = (PFNGLGETNAMEDBUFFERSUBDATAPROC) this->GetFunctionAddress("glGetNamedBufferSubData");
    this->_gl->glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC) this->GetFunctionAddress("glCreateFramebuffers");
    this->_gl->glNamedFramebufferRenderbuffer = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC) this->GetFunctionAddress("glNamedFramebufferRenderbuffer");
    this->_gl->glNamedFramebufferParameteri = (PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC) this->GetFunctionAddress("glNamedFramebufferParameteri");
    this->_gl->glNamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC) this->GetFunctionAddress("glNamedFramebufferTexture");
    this->_gl->glNamedFramebufferTextureLayer = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC) this->GetFunctionAddress("glNamedFramebufferTextureLayer");
    this->_gl->glNamedFramebufferDrawBuffer = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC) this->GetFunctionAddress("glNamedFramebufferDrawBuffer");
    this->_gl->glNamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC) this->GetFunctionAddress("glNamedFramebufferDrawBuffers");
    this->_gl->glNamedFramebufferReadBuffer = (PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC) this->GetFunctionAddress("glNamedFramebufferReadBuffer");
    this->_gl->glInvalidateNamedFramebufferData = (PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC) this->GetFunctionAddress("glInvalidateNamedFramebufferData");
    this->_gl->glInvalidateNamedFramebufferSubData = (PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC) this->GetFunctionAddress("glInvalidateNamedFramebufferSubData");
    this->_gl->glClearNamedFramebufferiv = (PFNGLCLEARNAMEDFRAMEBUFFERIVPROC) this->GetFunctionAddress("glClearNamedFramebufferiv");
    this->_gl->glClearNamedFramebufferuiv = (PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC) this->GetFunctionAddress("glClearNamedFramebufferuiv");
    this->_gl->glClearNamedFramebufferfv = (PFNGLCLEARNAMEDFRAMEBUFFERFVPROC) this->GetFunctionAddress("glClearNamedFramebufferfv");
    this->_gl->glClearNamedFramebufferfi = (PFNGLCLEARNAMEDFRAMEBUFFERFIPROC) this->GetFunctionAddress("glClearNamedFramebufferfi");
    this->_gl->glBlitNamedFramebuffer = (PFNGLBLITNAMEDFRAMEBUFFERPROC) this->GetFunctionAddress("glBlitNamedFramebuffer");
    this->_gl->glCheckNamedFramebufferStatus = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC) this->GetFunctionAddress("glCheckNamedFramebufferStatus");
    this->_gl->glGetNamedFramebufferParameteriv = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC) this->GetFunctionAddress("glGetNamedFramebufferParameteriv");
    this->_gl->glGetNamedFramebufferAttachmentParameteriv = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC) this->GetFunctionAddress("glGetNamedFramebufferAttachmentParameteriv");
    this->_gl->glCreateRenderbuffers = (PFNGLCREATERENDERBUFFERSPROC) this->GetFunctionAddress("glCreateRenderbuffers");
    this->_gl->glNamedRenderbufferStorage = (PFNGLNAMEDRENDERBUFFERSTORAGEPROC) this->GetFunctionAddress("glNamedRenderbufferStorage");
    this->_gl->glNamedRenderbufferStorageMultisample = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC) this->GetFunctionAddress("glNamedRenderbufferStorageMultisample");
    this->_gl->glGetNamedRenderbufferParameteriv = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC) this->GetFunctionAddress("glGetNamedRenderbufferParameteriv");
    this->_gl->glCreateTextures = (PFNGLCREATETEXTURESPROC) this->GetFunctionAddress("glCreateTextures");
    this->_gl->glTextureBuffer = (PFNGLTEXTUREBUFFERPROC) this->GetFunctionAddress("glTextureBuffer");
    this->_gl->glTextureBufferRange = (PFNGLTEXTUREBUFFERRANGEPROC) this->GetFunctionAddress("glTextureBufferRange");
    this->_gl->glTextureStorage1D = (PFNGLTEXTURESTORAGE1DPROC) this->GetFunctionAddress("glTextureStorage1D");
    this->_gl->glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC) this->GetFunctionAddress("glTextureStorage2D");
    this->_gl->glTextureStorage3D = (PFNGLTEXTURESTORAGE3DPROC) this->GetFunctionAddress("glTextureStorage3D");
    this->_gl->glTextureStorage2DMultisample = (PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC) this->GetFunctionAddress("glTextureStorage2DMultisample");
    this->_gl->glTextureStorage3DMultisample = (PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC) this->GetFunctionAddress("glTextureStorage3DMultisample");
    this->_gl->glTextureSubImage1D = (PFNGLTEXTURESUBIMAGE1DPROC) this->GetFunctionAddress("glTextureSubImage1D");
    this->_gl->glTextureSubImage2D = (PFNGLTEXTURESUBIMAGE2DPROC) this->GetFunctionAddress("glTextureSubImage2D");
    this->_gl->glTextureSubImage3D = (PFNGLTEXTURESUBIMAGE3DPROC) this->GetFunctionAddress("glTextureSubImage3D");
    this->_gl->glCompressedTextureSubImage1D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC) this->GetFunctionAddress("glCompressedTextureSubImage1D");
    this->_gl->glCompressedTextureSubImage2D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC) this->GetFunctionAddress("glCompressedTextureSubImage2D");
    this->_gl->glCompressedTextureSubImage3D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC) this->GetFunctionAddress("glCompressedTextureSubImage3D");
    this->_gl->glCopyTextureSubImage1D = (PFNGLCOPYTEXTURESUBIMAGE1DPROC) this->GetFunctionAddress("glCopyTextureSubImage1D");
    this->_gl->glCopyTextureSubImage2D = (PFNGLCOPYTEXTURESUBIMAGE2DPROC) this->GetFunctionAddress("glCopyTextureSubImage2D");
    this->_gl->glCopyTextureSubImage3D = (PFNGLCOPYTEXTURESUBIMAGE3DPROC) this->GetFunctionAddress("glCopyTextureSubImage3D");
    this->_gl->glTextureParameterf = (PFNGLTEXTUREPARAMETERFPROC) this->GetFunctionAddress("glTextureParameterf");
    this->_gl->glTextureParameterfv = (PFNGLTEXTUREPARAMETERFVPROC) this->GetFunctionAddress("glTextureParameterfv");
    this->_gl->glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC) this->GetFunctionAddress("glTextureParameteri");
    this->_gl->glTextureParameterIiv = (PFNGLTEXTUREPARAMETERIIVPROC) this->GetFunctionAddress("glTextureParameterIiv");
    this->_gl->glTextureParameterIuiv = (PFNGLTEXTUREPARAMETERIUIVPROC) this->GetFunctionAddress("glTextureParameterIuiv");
    this->_gl->glTextureParameteriv = (PFNGLTEXTUREPARAMETERIVPROC) this->GetFunctionAddress("glTextureParameteriv");
    this->_gl->glGenerateTextureMipmap = (PFNGLGENERATETEXTUREMIPMAPPROC) this->GetFunctionAddress("glGenerateTextureMipmap");
    this->_gl->glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC) this->GetFunctionAddress("glBindTextureUnit");
    this->_gl->glGetTextureImage = (PFNGLGETTEXTUREIMAGEPROC) this->GetFunctionAddress("glGetTextureImage");
    this->_gl->glGetCompressedTextureImage = (PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC) this->GetFunctionAddress("glGetCompressedTextureImage");
    this->_gl->glGetTextureLevelParameterfv = (PFNGLGETTEXTURELEVELPARAMETERFVPROC) this->GetFunctionAddress("glGetTextureLevelParameterfv");
    this->_gl->glGetTextureLevelParameteriv = (PFNGLGETTEXTURELEVELPARAMETERIVPROC) this->GetFunctionAddress("glGetTextureLevelParameteriv");
    this->_gl->glGetTextureParameterfv = (PFNGLGETTEXTUREPARAMETERFVPROC) this->GetFunctionAddress("glGetTextureParameterfv");
    this->_gl->glGetTextureParameterIiv = (PFNGLGETTEXTUREPARAMETERIIVPROC) this->GetFunctionAddress("glGetTextureParameterIiv");
    this->_gl->glGetTextureParameterIuiv = (PFNGLGETTEXTUREPARAMETERIUIVPROC) this->GetFunctionAddress("glGetTextureParameterIuiv");
    this->_gl->glGetTextureParameteriv = (PFNGLGETTEXTUREPARAMETERIVPROC) this->GetFunctionAddress("glGetTextureParameteriv");
    this->_gl->glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC) this->GetFunctionAddress("glCreateVertexArrays");
    this->_gl->glDisableVertexArrayAttrib = (PFNGLDISABLEVERTEXARRAYATTRIBPROC) this->GetFunctionAddress("glDisableVertexArrayAttrib");
    this->_gl->glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC) this->GetFunctionAddress("glEnableVertexArrayAttrib");
    this->_gl->glVertexArrayElementBuffer = (PFNGLVERTEXARRAYELEMENTBUFFERPROC) this->GetFunctionAddress("glVertexArrayElementBuffer");
    this->_gl->glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC) this->GetFunctionAddress("glVertexArrayVertexBuffer");
    this->_gl->glVertexArrayVertexBuffers = (PFNGLVERTEXARRAYVERTEXBUFFERSPROC) this->GetFunctionAddress("glVertexArrayVertexBuffers");
    this->_gl->glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC) this->GetFunctionAddress("glVertexArrayAttribBinding");
    this->_gl->glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC) this->GetFunctionAddress("glVertexArrayAttribFormat");
    this->_gl->glVertexArrayAttribIFormat = (PFNGLVERTEXARRAYATTRIBIFORMATPROC) this->GetFunctionAddress("glVertexArrayAttribIFormat");
    this->_gl->glVertexArrayAttribLFormat = (PFNGLVERTEXARRAYATTRIBLFORMATPROC) this->GetFunctionAddress("glVertexArrayAttribLFormat");
    this->_gl->glVertexArrayBindingDivisor = (PFNGLVERTEXARRAYBINDINGDIVISORPROC) this->GetFunctionAddress("glVertexArrayBindingDivisor");
    this->_gl->glGetVertexArrayiv = (PFNGLGETVERTEXARRAYIVPROC) this->GetFunctionAddress("glGetVertexArrayiv");
    this->_gl->glGetVertexArrayIndexediv = (PFNGLGETVERTEXARRAYINDEXEDIVPROC) this->GetFunctionAddress("glGetVertexArrayIndexediv");
    this->_gl->glGetVertexArrayIndexed64iv = (PFNGLGETVERTEXARRAYINDEXED64IVPROC) this->GetFunctionAddress("glGetVertexArrayIndexed64iv");
    this->_gl->glCreateSamplers = (PFNGLCREATESAMPLERSPROC) this->GetFunctionAddress("glCreateSamplers");
    this->_gl->glCreateProgramPipelines = (PFNGLCREATEPROGRAMPIPELINESPROC) this->GetFunctionAddress("glCreateProgramPipelines");
    this->_gl->glCreateQueries = (PFNGLCREATEQUERIESPROC) this->GetFunctionAddress("glCreateQueries");
    this->_gl->glGetQueryBufferObjecti64v = (PFNGLGETQUERYBUFFEROBJECTI64VPROC) this->GetFunctionAddress("glGetQueryBufferObjecti64v");
    this->_gl->glGetQueryBufferObjectiv = (PFNGLGETQUERYBUFFEROBJECTIVPROC) this->GetFunctionAddress("glGetQueryBufferObjectiv");
    this->_gl->glGetQueryBufferObjectui64v = (PFNGLGETQUERYBUFFEROBJECTUI64VPROC) this->GetFunctionAddress("glGetQueryBufferObjectui64v");
    this->_gl->glGetQueryBufferObjectuiv = (PFNGLGETQUERYBUFFEROBJECTUIVPROC) this->GetFunctionAddress("glGetQueryBufferObjectuiv");
    this->_gl->glMemoryBarrierByRegion = (PFNGLMEMORYBARRIERBYREGIONPROC) this->GetFunctionAddress("glMemoryBarrierByRegion");
    this->_gl->glGetTextureSubImage = (PFNGLGETTEXTURESUBIMAGEPROC) this->GetFunctionAddress("glGetTextureSubImage");
    this->_gl->glGetCompressedTextureSubImage = (PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC) this->GetFunctionAddress("glGetCompressedTextureSubImage");
    this->_gl->glGetGraphicsResetStatus = (PFNGLGETGRAPHICSRESETSTATUSPROC) this->GetFunctionAddress("glGetGraphicsResetStatus");
    this->_gl->glGetnCompressedTexImage = (PFNGLGETNCOMPRESSEDTEXIMAGEPROC) this->GetFunctionAddress("glGetnCompressedTexImage");
    this->_gl->glGetnTexImage = (PFNGLGETNTEXIMAGEPROC) this->GetFunctionAddress("glGetnTexImage");
    this->_gl->glGetnUniformdv = (PFNGLGETNUNIFORMDVPROC) this->GetFunctionAddress("glGetnUniformdv");
    this->_gl->glGetnUniformfv = (PFNGLGETNUNIFORMFVPROC) this->GetFunctionAddress("glGetnUniformfv");
    this->_gl->glGetnUniformiv = (PFNGLGETNUNIFORMIVPROC) this->GetFunctionAddress("glGetnUniformiv");
    this->_gl->glGetnUniformuiv = (PFNGLGETNUNIFORMUIVPROC) this->GetFunctionAddress("glGetnUniformuiv");
    this->_gl->glReadnPixels = (PFNGLREADNPIXELSPROC) this->GetFunctionAddress("glReadnPixels");
    this->_gl->glTextureBarrier = (PFNGLTEXTUREBARRIERPROC) this->GetFunctionAddress("glTextureBarrier");
    this->_gl->glSpecializeShader = (PFNGLSPECIALIZESHADERPROC) this->GetFunctionAddress("glSpecializeShader");
    this->_gl->glMultiDrawArraysIndirectCount = (PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC) this->GetFunctionAddress("glMultiDrawArraysIndirectCount");
    this->_gl->glMultiDrawElementsIndirectCount = (PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC) this->GetFunctionAddress("glMultiDrawElementsIndirectCount");
    this->_gl->glPolygonOffsetClamp = (PFNGLPOLYGONOFFSETCLAMPPROC) this->GetFunctionAddress("glPolygonOffsetClamp");
    this->_gl->glPrimitiveBoundingBoxARB = (PFNGLPRIMITIVEBOUNDINGBOXARBPROC) this->GetFunctionAddress("glPrimitiveBoundingBoxARB");
    this->_gl->glGetTextureHandleARB = (PFNGLGETTEXTUREHANDLEARBPROC) this->GetFunctionAddress("glGetTextureHandleARB");
    this->_gl->glGetTextureSamplerHandleARB = (PFNGLGETTEXTURESAMPLERHANDLEARBPROC) this->GetFunctionAddress("glGetTextureSamplerHandleARB");
    this->_gl->glMakeTextureHandleResidentARB = (PFNGLMAKETEXTUREHANDLERESIDENTARBPROC) this->GetFunctionAddress("glMakeTextureHandleResidentARB");
    this->_gl->glMakeTextureHandleNonResidentARB = (PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC) this->GetFunctionAddress("glMakeTextureHandleNonResidentARB");
    this->_gl->glGetImageHandleARB = (PFNGLGETIMAGEHANDLEARBPROC) this->GetFunctionAddress("glGetImageHandleARB");
    this->_gl->glMakeImageHandleResidentARB = (PFNGLMAKEIMAGEHANDLERESIDENTARBPROC) this->GetFunctionAddress("glMakeImageHandleResidentARB");
    this->_gl->glMakeImageHandleNonResidentARB = (PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC) this->GetFunctionAddress("glMakeImageHandleNonResidentARB");
    this->_gl->glUniformHandleui64ARB = (PFNGLUNIFORMHANDLEUI64ARBPROC) this->GetFunctionAddress("glUniformHandleui64ARB");
    this->_gl->glUniformHandleui64vARB = (PFNGLUNIFORMHANDLEUI64VARBPROC) this->GetFunctionAddress("glUniformHandleui64vARB");
    this->_gl->glProgramUniformHandleui64ARB = (PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC) this->GetFunctionAddress("glProgramUniformHandleui64ARB");
    this->_gl->glProgramUniformHandleui64vARB = (PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC) this->GetFunctionAddress("glProgramUniformHandleui64vARB");
    this->_gl->glIsTextureHandleResidentARB = (PFNGLISTEXTUREHANDLERESIDENTARBPROC) this->GetFunctionAddress("glIsTextureHandleResidentARB");
    this->_gl->glIsImageHandleResidentARB = (PFNGLISIMAGEHANDLERESIDENTARBPROC) this->GetFunctionAddress("glIsImageHandleResidentARB");
    this->_gl->glVertexAttribL1ui64ARB = (PFNGLVERTEXATTRIBL1UI64ARBPROC) this->GetFunctionAddress("glVertexAttribL1ui64ARB");
    this->_gl->glVertexAttribL1ui64vARB = (PFNGLVERTEXATTRIBL1UI64VARBPROC) this->GetFunctionAddress("glVertexAttribL1ui64vARB");
    this->_gl->glGetVertexAttribLui64vARB = (PFNGLGETVERTEXATTRIBLUI64VARBPROC) this->GetFunctionAddress("glGetVertexAttribLui64vARB");
    this->_gl->glCreateSyncFromCLeventARB = (PFNGLCREATESYNCFROMCLEVENTARBPROC) this->GetFunctionAddress("glCreateSyncFromCLeventARB");
    this->_gl->glDispatchComputeGroupSizeARB = (PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC) this->GetFunctionAddress("glDispatchComputeGroupSizeARB");
    this->_gl->glDebugMessageControlARB = (PFNGLDEBUGMESSAGECONTROLARBPROC) this->GetFunctionAddress("glDebugMessageControlARB");
    this->_gl->glDebugMessageInsertARB = (PFNGLDEBUGMESSAGEINSERTARBPROC) this->GetFunctionAddress("glDebugMessageInsertARB");
    this->_gl->glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC) this->GetFunctionAddress("glDebugMessageCallbackARB");
    this->_gl->glGetDebugMessageLogARB = (PFNGLGETDEBUGMESSAGELOGARBPROC) this->GetFunctionAddress("glGetDebugMessageLogARB");
    this->_gl->glBlendEquationiARB = (PFNGLBLENDEQUATIONIARBPROC) this->GetFunctionAddress("glBlendEquationiARB");
    this->_gl->glBlendEquationSeparateiARB = (PFNGLBLENDEQUATIONSEPARATEIARBPROC) this->GetFunctionAddress("glBlendEquationSeparateiARB");
    this->_gl->glBlendFunciARB = (PFNGLBLENDFUNCIARBPROC) this->GetFunctionAddress("glBlendFunciARB");
    this->_gl->glBlendFuncSeparateiARB = (PFNGLBLENDFUNCSEPARATEIARBPROC) this->GetFunctionAddress("glBlendFuncSeparateiARB");
    this->_gl->glDrawArraysInstancedARB = (PFNGLDRAWARRAYSINSTANCEDARBPROC) this->GetFunctionAddress("glDrawArraysInstancedARB");
    this->_gl->glDrawElementsInstancedARB = (PFNGLDRAWELEMENTSINSTANCEDARBPROC) this->GetFunctionAddress("glDrawElementsInstancedARB");
    this->_gl->glProgramParameteriARB = (PFNGLPROGRAMPARAMETERIARBPROC) this->GetFunctionAddress("glProgramParameteriARB");
    this->_gl->glFramebufferTextureARB = (PFNGLFRAMEBUFFERTEXTUREARBPROC) this->GetFunctionAddress("glFramebufferTextureARB");
    this->_gl->glFramebufferTextureLayerARB = (PFNGLFRAMEBUFFERTEXTURELAYERARBPROC) this->GetFunctionAddress("glFramebufferTextureLayerARB");
    this->_gl->glFramebufferTextureFaceARB = (PFNGLFRAMEBUFFERTEXTUREFACEARBPROC) this->GetFunctionAddress("glFramebufferTextureFaceARB");
    this->_gl->glSpecializeShaderARB = (PFNGLSPECIALIZESHADERARBPROC) this->GetFunctionAddress("glSpecializeShaderARB");
    this->_gl->glUniform1i64ARB = (PFNGLUNIFORM1I64ARBPROC) this->GetFunctionAddress("glUniform1i64ARB");
    this->_gl->glUniform2i64ARB = (PFNGLUNIFORM2I64ARBPROC) this->GetFunctionAddress("glUniform2i64ARB");
    this->_gl->glUniform3i64ARB = (PFNGLUNIFORM3I64ARBPROC) this->GetFunctionAddress("glUniform3i64ARB");
    this->_gl->glUniform4i64ARB = (PFNGLUNIFORM4I64ARBPROC) this->GetFunctionAddress("glUniform4i64ARB");
    this->_gl->glUniform1i64vARB = (PFNGLUNIFORM1I64VARBPROC) this->GetFunctionAddress("glUniform1i64vARB");
    this->_gl->glUniform2i64vARB = (PFNGLUNIFORM2I64VARBPROC) this->GetFunctionAddress("glUniform2i64vARB");
    this->_gl->glUniform3i64vARB = (PFNGLUNIFORM3I64VARBPROC) this->GetFunctionAddress("glUniform3i64vARB");
    this->_gl->glUniform4i64vARB = (PFNGLUNIFORM4I64VARBPROC) this->GetFunctionAddress("glUniform4i64vARB");
    this->_gl->glUniform1ui64ARB = (PFNGLUNIFORM1UI64ARBPROC) this->GetFunctionAddress("glUniform1ui64ARB");
    this->_gl->glUniform2ui64ARB = (PFNGLUNIFORM2UI64ARBPROC) this->GetFunctionAddress("glUniform2ui64ARB");
    this->_gl->glUniform3ui64ARB = (PFNGLUNIFORM3UI64ARBPROC) this->GetFunctionAddress("glUniform3ui64ARB");
    this->_gl->glUniform4ui64ARB = (PFNGLUNIFORM4UI64ARBPROC) this->GetFunctionAddress("glUniform4ui64ARB");
    this->_gl->glUniform1ui64vARB = (PFNGLUNIFORM1UI64VARBPROC) this->GetFunctionAddress("glUniform1ui64vARB");
    this->_gl->glUniform2ui64vARB = (PFNGLUNIFORM2UI64VARBPROC) this->GetFunctionAddress("glUniform2ui64vARB");
    this->_gl->glUniform3ui64vARB = (PFNGLUNIFORM3UI64VARBPROC) this->GetFunctionAddress("glUniform3ui64vARB");
    this->_gl->glUniform4ui64vARB = (PFNGLUNIFORM4UI64VARBPROC) this->GetFunctionAddress("glUniform4ui64vARB");
    this->_gl->glGetUniformi64vARB = (PFNGLGETUNIFORMI64VARBPROC) this->GetFunctionAddress("glGetUniformi64vARB");
    this->_gl->glGetUniformui64vARB = (PFNGLGETUNIFORMUI64VARBPROC) this->GetFunctionAddress("glGetUniformui64vARB");
    this->_gl->glGetnUniformi64vARB = (PFNGLGETNUNIFORMI64VARBPROC) this->GetFunctionAddress("glGetnUniformi64vARB");
    this->_gl->glGetnUniformui64vARB = (PFNGLGETNUNIFORMUI64VARBPROC) this->GetFunctionAddress("glGetnUniformui64vARB");
    this->_gl->glProgramUniform1i64ARB = (PFNGLPROGRAMUNIFORM1I64ARBPROC) this->GetFunctionAddress("glProgramUniform1i64ARB");
    this->_gl->glProgramUniform2i64ARB = (PFNGLPROGRAMUNIFORM2I64ARBPROC) this->GetFunctionAddress("glProgramUniform2i64ARB");
    this->_gl->glProgramUniform3i64ARB = (PFNGLPROGRAMUNIFORM3I64ARBPROC) this->GetFunctionAddress("glProgramUniform3i64ARB");
    this->_gl->glProgramUniform4i64ARB = (PFNGLPROGRAMUNIFORM4I64ARBPROC) this->GetFunctionAddress("glProgramUniform4i64ARB");
    this->_gl->glProgramUniform1i64vARB = (PFNGLPROGRAMUNIFORM1I64VARBPROC) this->GetFunctionAddress("glProgramUniform1i64vARB");
    this->_gl->glProgramUniform2i64vARB = (PFNGLPROGRAMUNIFORM2I64VARBPROC) this->GetFunctionAddress("glProgramUniform2i64vARB");
    this->_gl->glProgramUniform3i64vARB = (PFNGLPROGRAMUNIFORM3I64VARBPROC) this->GetFunctionAddress("glProgramUniform3i64vARB");
    this->_gl->glProgramUniform4i64vARB = (PFNGLPROGRAMUNIFORM4I64VARBPROC) this->GetFunctionAddress("glProgramUniform4i64vARB");
    this->_gl->glProgramUniform1ui64ARB = (PFNGLPROGRAMUNIFORM1UI64ARBPROC) this->GetFunctionAddress("glProgramUniform1ui64ARB");
    this->_gl->glProgramUniform2ui64ARB = (PFNGLPROGRAMUNIFORM2UI64ARBPROC) this->GetFunctionAddress("glProgramUniform2ui64ARB");
    this->_gl->glProgramUniform3ui64ARB = (PFNGLPROGRAMUNIFORM3UI64ARBPROC) this->GetFunctionAddress("glProgramUniform3ui64ARB");
    this->_gl->glProgramUniform4ui64ARB = (PFNGLPROGRAMUNIFORM4UI64ARBPROC) this->GetFunctionAddress("glProgramUniform4ui64ARB");
    this->_gl->glProgramUniform1ui64vARB = (PFNGLPROGRAMUNIFORM1UI64VARBPROC) this->GetFunctionAddress("glProgramUniform1ui64vARB");
    this->_gl->glProgramUniform2ui64vARB = (PFNGLPROGRAMUNIFORM2UI64VARBPROC) this->GetFunctionAddress("glProgramUniform2ui64vARB");
    this->_gl->glProgramUniform3ui64vARB = (PFNGLPROGRAMUNIFORM3UI64VARBPROC) this->GetFunctionAddress("glProgramUniform3ui64vARB");
    this->_gl->glProgramUniform4ui64vARB = (PFNGLPROGRAMUNIFORM4UI64VARBPROC) this->GetFunctionAddress("glProgramUniform4ui64vARB");
    this->_gl->glMultiDrawArraysIndirectCountARB = (PFNGLMULTIDRAWARRAYSINDIRECTCOUNTARBPROC) this->GetFunctionAddress("glMultiDrawArraysIndirectCountARB");
    this->_gl->glMultiDrawElementsIndirectCountARB = (PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTARBPROC) this->GetFunctionAddress("glMultiDrawElementsIndirectCountARB");
    this->_gl->glVertexAttribDivisorARB = (PFNGLVERTEXATTRIBDIVISORARBPROC) this->GetFunctionAddress("glVertexAttribDivisorARB");
    this->_gl->glMaxShaderCompilerThreadsARB = (PFNGLMAXSHADERCOMPILERTHREADSARBPROC) this->GetFunctionAddress("glMaxShaderCompilerThreadsARB");
    this->_gl->glGetGraphicsResetStatusARB = (PFNGLGETGRAPHICSRESETSTATUSARBPROC) this->GetFunctionAddress("glGetGraphicsResetStatusARB");
    this->_gl->glGetnTexImageARB = (PFNGLGETNTEXIMAGEARBPROC) this->GetFunctionAddress("glGetnTexImageARB");
    this->_gl->glReadnPixelsARB = (PFNGLREADNPIXELSARBPROC) this->GetFunctionAddress("glReadnPixelsARB");
    this->_gl->glGetnCompressedTexImageARB = (PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC) this->GetFunctionAddress("glGetnCompressedTexImageARB");
    this->_gl->glGetnUniformfvARB = (PFNGLGETNUNIFORMFVARBPROC) this->GetFunctionAddress("glGetnUniformfvARB");
    this->_gl->glGetnUniformivARB = (PFNGLGETNUNIFORMIVARBPROC) this->GetFunctionAddress("glGetnUniformivARB");
    this->_gl->glGetnUniformuivARB = (PFNGLGETNUNIFORMUIVARBPROC) this->GetFunctionAddress("glGetnUniformuivARB");
    this->_gl->glGetnUniformdvARB = (PFNGLGETNUNIFORMDVARBPROC) this->GetFunctionAddress("glGetnUniformdvARB");
    this->_gl->glFramebufferSampleLocationsfvARB = (PFNGLFRAMEBUFFERSAMPLELOCATIONSFVARBPROC) this->GetFunctionAddress("glFramebufferSampleLocationsfvARB");
    this->_gl->glNamedFramebufferSampleLocationsfvARB = (PFNGLNAMEDFRAMEBUFFERSAMPLELOCATIONSFVARBPROC) this->GetFunctionAddress("glNamedFramebufferSampleLocationsfvARB");
    this->_gl->glEvaluateDepthValuesARB = (PFNGLEVALUATEDEPTHVALUESARBPROC) this->GetFunctionAddress("glEvaluateDepthValuesARB");
    this->_gl->glMinSampleShadingARB = (PFNGLMINSAMPLESHADINGARBPROC) this->GetFunctionAddress("glMinSampleShadingARB");
    this->_gl->glNamedStringARB = (PFNGLNAMEDSTRINGARBPROC) this->GetFunctionAddress("glNamedStringARB");
    this->_gl->glDeleteNamedStringARB = (PFNGLDELETENAMEDSTRINGARBPROC) this->GetFunctionAddress("glDeleteNamedStringARB");
    this->_gl->glCompileShaderIncludeARB = (PFNGLCOMPILESHADERINCLUDEARBPROC) this->GetFunctionAddress("glCompileShaderIncludeARB");
    this->_gl->glIsNamedStringARB = (PFNGLISNAMEDSTRINGARBPROC) this->GetFunctionAddress("glIsNamedStringARB");
    this->_gl->glGetNamedStringARB = (PFNGLGETNAMEDSTRINGARBPROC) this->GetFunctionAddress("glGetNamedStringARB");
    this->_gl->glGetNamedStringivARB = (PFNGLGETNAMEDSTRINGIVARBPROC) this->GetFunctionAddress("glGetNamedStringivARB");
    this->_gl->glBufferPageCommitmentARB = (PFNGLBUFFERPAGECOMMITMENTARBPROC) this->GetFunctionAddress("glBufferPageCommitmentARB");
    this->_gl->glNamedBufferPageCommitmentEXT = (PFNGLNAMEDBUFFERPAGECOMMITMENTEXTPROC) this->GetFunctionAddress("glNamedBufferPageCommitmentEXT");
    this->_gl->glNamedBufferPageCommitmentARB = (PFNGLNAMEDBUFFERPAGECOMMITMENTARBPROC) this->GetFunctionAddress("glNamedBufferPageCommitmentARB");
    this->_gl->glTexPageCommitmentARB = (PFNGLTEXPAGECOMMITMENTARBPROC) this->GetFunctionAddress("glTexPageCommitmentARB");
    this->_gl->glTexBufferARB = (PFNGLTEXBUFFERARBPROC) this->GetFunctionAddress("glTexBufferARB");
    this->_gl->glDepthRangeArraydvNV = (PFNGLDEPTHRANGEARRAYDVNVPROC) this->GetFunctionAddress("glDepthRangeArraydvNV");
    this->_gl->glDepthRangeIndexeddNV = (PFNGLDEPTHRANGEINDEXEDDNVPROC) this->GetFunctionAddress("glDepthRangeIndexeddNV");
    this->_gl->glBlendBarrierKHR = (PFNGLBLENDBARRIERKHRPROC) this->GetFunctionAddress("glBlendBarrierKHR");
    this->_gl->glMaxShaderCompilerThreadsKHR = (PFNGLMAXSHADERCOMPILERTHREADSKHRPROC) this->GetFunctionAddress("glMaxShaderCompilerThreadsKHR");
    this->_gl->glRenderbufferStorageMultisampleAdvancedAMD = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEADVANCEDAMDPROC) this->GetFunctionAddress("glRenderbufferStorageMultisampleAdvancedAMD");
    this->_gl->glNamedRenderbufferStorageMultisampleAdvancedAMD = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEADVANCEDAMDPROC) this->GetFunctionAddress("glNamedRenderbufferStorageMultisampleAdvancedAMD");
    this->_gl->glGetPerfMonitorGroupsAMD = (PFNGLGETPERFMONITORGROUPSAMDPROC) this->GetFunctionAddress("glGetPerfMonitorGroupsAMD");
    this->_gl->glGetPerfMonitorCountersAMD = (PFNGLGETPERFMONITORCOUNTERSAMDPROC) this->GetFunctionAddress("glGetPerfMonitorCountersAMD");
    this->_gl->glGetPerfMonitorGroupStringAMD = (PFNGLGETPERFMONITORGROUPSTRINGAMDPROC) this->GetFunctionAddress("glGetPerfMonitorGroupStringAMD");
    this->_gl->glGetPerfMonitorCounterStringAMD = (PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC) this->GetFunctionAddress("glGetPerfMonitorCounterStringAMD");
    this->_gl->glGetPerfMonitorCounterInfoAMD = (PFNGLGETPERFMONITORCOUNTERINFOAMDPROC) this->GetFunctionAddress("glGetPerfMonitorCounterInfoAMD");
    this->_gl->glGenPerfMonitorsAMD = (PFNGLGENPERFMONITORSAMDPROC) this->GetFunctionAddress("glGenPerfMonitorsAMD");
    this->_gl->glDeletePerfMonitorsAMD = (PFNGLDELETEPERFMONITORSAMDPROC) this->GetFunctionAddress("glDeletePerfMonitorsAMD");
    this->_gl->glSelectPerfMonitorCountersAMD = (PFNGLSELECTPERFMONITORCOUNTERSAMDPROC) this->GetFunctionAddress("glSelectPerfMonitorCountersAMD");
    this->_gl->glBeginPerfMonitorAMD = (PFNGLBEGINPERFMONITORAMDPROC) this->GetFunctionAddress("glBeginPerfMonitorAMD");
    this->_gl->glEndPerfMonitorAMD = (PFNGLENDPERFMONITORAMDPROC) this->GetFunctionAddress("glEndPerfMonitorAMD");
    this->_gl->glGetPerfMonitorCounterDataAMD = (PFNGLGETPERFMONITORCOUNTERDATAAMDPROC) this->GetFunctionAddress("glGetPerfMonitorCounterDataAMD");
    this->_gl->glEGLImageTargetTexStorageEXT = (PFNGLEGLIMAGETARGETTEXSTORAGEEXTPROC) this->GetFunctionAddress("glEGLImageTargetTexStorageEXT");
    this->_gl->glEGLImageTargetTextureStorageEXT = (PFNGLEGLIMAGETARGETTEXTURESTORAGEEXTPROC) this->GetFunctionAddress("glEGLImageTargetTextureStorageEXT");
    this->_gl->glLabelObjectEXT = (PFNGLLABELOBJECTEXTPROC) this->GetFunctionAddress("glLabelObjectEXT");
    this->_gl->glGetObjectLabelEXT = (PFNGLGETOBJECTLABELEXTPROC) this->GetFunctionAddress("glGetObjectLabelEXT");
    this->_gl->glInsertEventMarkerEXT = (PFNGLINSERTEVENTMARKEREXTPROC) this->GetFunctionAddress("glInsertEventMarkerEXT");
    this->_gl->glPushGroupMarkerEXT = (PFNGLPUSHGROUPMARKEREXTPROC) this->GetFunctionAddress("glPushGroupMarkerEXT");
    this->_gl->glPopGroupMarkerEXT = (PFNGLPOPGROUPMARKEREXTPROC) this->GetFunctionAddress("glPopGroupMarkerEXT");
    this->_gl->glMatrixLoadfEXT = (PFNGLMATRIXLOADFEXTPROC) this->GetFunctionAddress("glMatrixLoadfEXT");
    this->_gl->glMatrixLoaddEXT = (PFNGLMATRIXLOADDEXTPROC) this->GetFunctionAddress("glMatrixLoaddEXT");
    this->_gl->glMatrixMultfEXT = (PFNGLMATRIXMULTFEXTPROC) this->GetFunctionAddress("glMatrixMultfEXT");
    this->_gl->glMatrixMultdEXT = (PFNGLMATRIXMULTDEXTPROC) this->GetFunctionAddress("glMatrixMultdEXT");
    this->_gl->glMatrixLoadIdentityEXT = (PFNGLMATRIXLOADIDENTITYEXTPROC) this->GetFunctionAddress("glMatrixLoadIdentityEXT");
    this->_gl->glMatrixRotatefEXT = (PFNGLMATRIXROTATEFEXTPROC) this->GetFunctionAddress("glMatrixRotatefEXT");
    this->_gl->glMatrixRotatedEXT = (PFNGLMATRIXROTATEDEXTPROC) this->GetFunctionAddress("glMatrixRotatedEXT");
    this->_gl->glMatrixScalefEXT = (PFNGLMATRIXSCALEFEXTPROC) this->GetFunctionAddress("glMatrixScalefEXT");
    this->_gl->glMatrixScaledEXT = (PFNGLMATRIXSCALEDEXTPROC) this->GetFunctionAddress("glMatrixScaledEXT");
    this->_gl->glMatrixTranslatefEXT = (PFNGLMATRIXTRANSLATEFEXTPROC) this->GetFunctionAddress("glMatrixTranslatefEXT");
    this->_gl->glMatrixTranslatedEXT = (PFNGLMATRIXTRANSLATEDEXTPROC) this->GetFunctionAddress("glMatrixTranslatedEXT");
    this->_gl->glMatrixFrustumEXT = (PFNGLMATRIXFRUSTUMEXTPROC) this->GetFunctionAddress("glMatrixFrustumEXT");
    this->_gl->glMatrixOrthoEXT = (PFNGLMATRIXORTHOEXTPROC) this->GetFunctionAddress("glMatrixOrthoEXT");
    this->_gl->glMatrixPopEXT = (PFNGLMATRIXPOPEXTPROC) this->GetFunctionAddress("glMatrixPopEXT");
    this->_gl->glMatrixPushEXT = (PFNGLMATRIXPUSHEXTPROC) this->GetFunctionAddress("glMatrixPushEXT");
    this->_gl->glClientAttribDefaultEXT = (PFNGLCLIENTATTRIBDEFAULTEXTPROC) this->GetFunctionAddress("glClientAttribDefaultEXT");
    this->_gl->glPushClientAttribDefaultEXT = (PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC) this->GetFunctionAddress("glPushClientAttribDefaultEXT");
    this->_gl->glTextureParameterfEXT = (PFNGLTEXTUREPARAMETERFEXTPROC) this->GetFunctionAddress("glTextureParameterfEXT");
    this->_gl->glTextureParameterfvEXT = (PFNGLTEXTUREPARAMETERFVEXTPROC) this->GetFunctionAddress("glTextureParameterfvEXT");
    this->_gl->glTextureParameteriEXT = (PFNGLTEXTUREPARAMETERIEXTPROC) this->GetFunctionAddress("glTextureParameteriEXT");
    this->_gl->glTextureParameterivEXT = (PFNGLTEXTUREPARAMETERIVEXTPROC) this->GetFunctionAddress("glTextureParameterivEXT");
    this->_gl->glTextureImage1DEXT = (PFNGLTEXTUREIMAGE1DEXTPROC) this->GetFunctionAddress("glTextureImage1DEXT");
    this->_gl->glTextureImage2DEXT = (PFNGLTEXTUREIMAGE2DEXTPROC) this->GetFunctionAddress("glTextureImage2DEXT");
    this->_gl->glTextureSubImage1DEXT = (PFNGLTEXTURESUBIMAGE1DEXTPROC) this->GetFunctionAddress("glTextureSubImage1DEXT");
    this->_gl->glTextureSubImage2DEXT = (PFNGLTEXTURESUBIMAGE2DEXTPROC) this->GetFunctionAddress("glTextureSubImage2DEXT");
    this->_gl->glCopyTextureImage1DEXT = (PFNGLCOPYTEXTUREIMAGE1DEXTPROC) this->GetFunctionAddress("glCopyTextureImage1DEXT");
    this->_gl->glCopyTextureImage2DEXT = (PFNGLCOPYTEXTUREIMAGE2DEXTPROC) this->GetFunctionAddress("glCopyTextureImage2DEXT");
    this->_gl->glCopyTextureSubImage1DEXT = (PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC) this->GetFunctionAddress("glCopyTextureSubImage1DEXT");
    this->_gl->glCopyTextureSubImage2DEXT = (PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC) this->GetFunctionAddress("glCopyTextureSubImage2DEXT");
    this->_gl->glGetTextureImageEXT = (PFNGLGETTEXTUREIMAGEEXTPROC) this->GetFunctionAddress("glGetTextureImageEXT");
    this->_gl->glGetTextureParameterfvEXT = (PFNGLGETTEXTUREPARAMETERFVEXTPROC) this->GetFunctionAddress("glGetTextureParameterfvEXT");
    this->_gl->glGetTextureParameterivEXT = (PFNGLGETTEXTUREPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetTextureParameterivEXT");
    this->_gl->glGetTextureLevelParameterfvEXT = (PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC) this->GetFunctionAddress("glGetTextureLevelParameterfvEXT");
    this->_gl->glGetTextureLevelParameterivEXT = (PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetTextureLevelParameterivEXT");
    this->_gl->glTextureImage3DEXT = (PFNGLTEXTUREIMAGE3DEXTPROC) this->GetFunctionAddress("glTextureImage3DEXT");
    this->_gl->glTextureSubImage3DEXT = (PFNGLTEXTURESUBIMAGE3DEXTPROC) this->GetFunctionAddress("glTextureSubImage3DEXT");
    this->_gl->glCopyTextureSubImage3DEXT = (PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC) this->GetFunctionAddress("glCopyTextureSubImage3DEXT");
    this->_gl->glBindMultiTextureEXT = (PFNGLBINDMULTITEXTUREEXTPROC) this->GetFunctionAddress("glBindMultiTextureEXT");
    this->_gl->glMultiTexCoordPointerEXT = (PFNGLMULTITEXCOORDPOINTEREXTPROC) this->GetFunctionAddress("glMultiTexCoordPointerEXT");
    this->_gl->glMultiTexEnvfEXT = (PFNGLMULTITEXENVFEXTPROC) this->GetFunctionAddress("glMultiTexEnvfEXT");
    this->_gl->glMultiTexEnvfvEXT = (PFNGLMULTITEXENVFVEXTPROC) this->GetFunctionAddress("glMultiTexEnvfvEXT");
    this->_gl->glMultiTexEnviEXT = (PFNGLMULTITEXENVIEXTPROC) this->GetFunctionAddress("glMultiTexEnviEXT");
    this->_gl->glMultiTexEnvivEXT = (PFNGLMULTITEXENVIVEXTPROC) this->GetFunctionAddress("glMultiTexEnvivEXT");
    this->_gl->glMultiTexGendEXT = (PFNGLMULTITEXGENDEXTPROC) this->GetFunctionAddress("glMultiTexGendEXT");
    this->_gl->glMultiTexGendvEXT = (PFNGLMULTITEXGENDVEXTPROC) this->GetFunctionAddress("glMultiTexGendvEXT");
    this->_gl->glMultiTexGenfEXT = (PFNGLMULTITEXGENFEXTPROC) this->GetFunctionAddress("glMultiTexGenfEXT");
    this->_gl->glMultiTexGenfvEXT = (PFNGLMULTITEXGENFVEXTPROC) this->GetFunctionAddress("glMultiTexGenfvEXT");
    this->_gl->glMultiTexGeniEXT = (PFNGLMULTITEXGENIEXTPROC) this->GetFunctionAddress("glMultiTexGeniEXT");
    this->_gl->glMultiTexGenivEXT = (PFNGLMULTITEXGENIVEXTPROC) this->GetFunctionAddress("glMultiTexGenivEXT");
    this->_gl->glGetMultiTexEnvfvEXT = (PFNGLGETMULTITEXENVFVEXTPROC) this->GetFunctionAddress("glGetMultiTexEnvfvEXT");
    this->_gl->glGetMultiTexEnvivEXT = (PFNGLGETMULTITEXENVIVEXTPROC) this->GetFunctionAddress("glGetMultiTexEnvivEXT");
    this->_gl->glGetMultiTexGendvEXT = (PFNGLGETMULTITEXGENDVEXTPROC) this->GetFunctionAddress("glGetMultiTexGendvEXT");
    this->_gl->glGetMultiTexGenfvEXT = (PFNGLGETMULTITEXGENFVEXTPROC) this->GetFunctionAddress("glGetMultiTexGenfvEXT");
    this->_gl->glGetMultiTexGenivEXT = (PFNGLGETMULTITEXGENIVEXTPROC) this->GetFunctionAddress("glGetMultiTexGenivEXT");
    this->_gl->glMultiTexParameteriEXT = (PFNGLMULTITEXPARAMETERIEXTPROC) this->GetFunctionAddress("glMultiTexParameteriEXT");
    this->_gl->glMultiTexParameterivEXT = (PFNGLMULTITEXPARAMETERIVEXTPROC) this->GetFunctionAddress("glMultiTexParameterivEXT");
    this->_gl->glMultiTexParameterfEXT = (PFNGLMULTITEXPARAMETERFEXTPROC) this->GetFunctionAddress("glMultiTexParameterfEXT");
    this->_gl->glMultiTexParameterfvEXT = (PFNGLMULTITEXPARAMETERFVEXTPROC) this->GetFunctionAddress("glMultiTexParameterfvEXT");
    this->_gl->glMultiTexImage1DEXT = (PFNGLMULTITEXIMAGE1DEXTPROC) this->GetFunctionAddress("glMultiTexImage1DEXT");
    this->_gl->glMultiTexImage2DEXT = (PFNGLMULTITEXIMAGE2DEXTPROC) this->GetFunctionAddress("glMultiTexImage2DEXT");
    this->_gl->glMultiTexSubImage1DEXT = (PFNGLMULTITEXSUBIMAGE1DEXTPROC) this->GetFunctionAddress("glMultiTexSubImage1DEXT");
    this->_gl->glMultiTexSubImage2DEXT = (PFNGLMULTITEXSUBIMAGE2DEXTPROC) this->GetFunctionAddress("glMultiTexSubImage2DEXT");
    this->_gl->glCopyMultiTexImage1DEXT = (PFNGLCOPYMULTITEXIMAGE1DEXTPROC) this->GetFunctionAddress("glCopyMultiTexImage1DEXT");
    this->_gl->glCopyMultiTexImage2DEXT = (PFNGLCOPYMULTITEXIMAGE2DEXTPROC) this->GetFunctionAddress("glCopyMultiTexImage2DEXT");
    this->_gl->glCopyMultiTexSubImage1DEXT = (PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC) this->GetFunctionAddress("glCopyMultiTexSubImage1DEXT");
    this->_gl->glCopyMultiTexSubImage2DEXT = (PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC) this->GetFunctionAddress("glCopyMultiTexSubImage2DEXT");
    this->_gl->glGetMultiTexImageEXT = (PFNGLGETMULTITEXIMAGEEXTPROC) this->GetFunctionAddress("glGetMultiTexImageEXT");
    this->_gl->glGetMultiTexParameterfvEXT = (PFNGLGETMULTITEXPARAMETERFVEXTPROC) this->GetFunctionAddress("glGetMultiTexParameterfvEXT");
    this->_gl->glGetMultiTexParameterivEXT = (PFNGLGETMULTITEXPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetMultiTexParameterivEXT");
    this->_gl->glGetMultiTexLevelParameterfvEXT = (PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC) this->GetFunctionAddress("glGetMultiTexLevelParameterfvEXT");
    this->_gl->glGetMultiTexLevelParameterivEXT = (PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetMultiTexLevelParameterivEXT");
    this->_gl->glMultiTexImage3DEXT = (PFNGLMULTITEXIMAGE3DEXTPROC) this->GetFunctionAddress("glMultiTexImage3DEXT");
    this->_gl->glMultiTexSubImage3DEXT = (PFNGLMULTITEXSUBIMAGE3DEXTPROC) this->GetFunctionAddress("glMultiTexSubImage3DEXT");
    this->_gl->glCopyMultiTexSubImage3DEXT = (PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC) this->GetFunctionAddress("glCopyMultiTexSubImage3DEXT");
    this->_gl->glEnableClientStateIndexedEXT = (PFNGLENABLECLIENTSTATEINDEXEDEXTPROC) this->GetFunctionAddress("glEnableClientStateIndexedEXT");
    this->_gl->glDisableClientStateIndexedEXT = (PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC) this->GetFunctionAddress("glDisableClientStateIndexedEXT");
    this->_gl->glGetFloatIndexedvEXT = (PFNGLGETFLOATINDEXEDVEXTPROC) this->GetFunctionAddress("glGetFloatIndexedvEXT");
    this->_gl->glGetDoubleIndexedvEXT = (PFNGLGETDOUBLEINDEXEDVEXTPROC) this->GetFunctionAddress("glGetDoubleIndexedvEXT");
    this->_gl->glGetPointerIndexedvEXT = (PFNGLGETPOINTERINDEXEDVEXTPROC) this->GetFunctionAddress("glGetPointerIndexedvEXT");
    this->_gl->glEnableIndexedEXT = (PFNGLENABLEINDEXEDEXTPROC) this->GetFunctionAddress("glEnableIndexedEXT");
    this->_gl->glDisableIndexedEXT = (PFNGLDISABLEINDEXEDEXTPROC) this->GetFunctionAddress("glDisableIndexedEXT");
    this->_gl->glIsEnabledIndexedEXT = (PFNGLISENABLEDINDEXEDEXTPROC) this->GetFunctionAddress("glIsEnabledIndexedEXT");
    this->_gl->glGetIntegerIndexedvEXT = (PFNGLGETINTEGERINDEXEDVEXTPROC) this->GetFunctionAddress("glGetIntegerIndexedvEXT");
    this->_gl->glGetBooleanIndexedvEXT = (PFNGLGETBOOLEANINDEXEDVEXTPROC) this->GetFunctionAddress("glGetBooleanIndexedvEXT");
    this->_gl->glCompressedTextureImage3DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC) this->GetFunctionAddress("glCompressedTextureImage3DEXT");
    this->_gl->glCompressedTextureImage2DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC) this->GetFunctionAddress("glCompressedTextureImage2DEXT");
    this->_gl->glCompressedTextureImage1DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC) this->GetFunctionAddress("glCompressedTextureImage1DEXT");
    this->_gl->glCompressedTextureSubImage3DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC) this->GetFunctionAddress("glCompressedTextureSubImage3DEXT");
    this->_gl->glCompressedTextureSubImage2DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC) this->GetFunctionAddress("glCompressedTextureSubImage2DEXT");
    this->_gl->glCompressedTextureSubImage1DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC) this->GetFunctionAddress("glCompressedTextureSubImage1DEXT");
    this->_gl->glGetCompressedTextureImageEXT = (PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC) this->GetFunctionAddress("glGetCompressedTextureImageEXT");
    this->_gl->glCompressedMultiTexImage3DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC) this->GetFunctionAddress("glCompressedMultiTexImage3DEXT");
    this->_gl->glCompressedMultiTexImage2DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC) this->GetFunctionAddress("glCompressedMultiTexImage2DEXT");
    this->_gl->glCompressedMultiTexImage1DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC) this->GetFunctionAddress("glCompressedMultiTexImage1DEXT");
    this->_gl->glCompressedMultiTexSubImage3DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC) this->GetFunctionAddress("glCompressedMultiTexSubImage3DEXT");
    this->_gl->glCompressedMultiTexSubImage2DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC) this->GetFunctionAddress("glCompressedMultiTexSubImage2DEXT");
    this->_gl->glCompressedMultiTexSubImage1DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC) this->GetFunctionAddress("glCompressedMultiTexSubImage1DEXT");
    this->_gl->glGetCompressedMultiTexImageEXT = (PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC) this->GetFunctionAddress("glGetCompressedMultiTexImageEXT");
    this->_gl->glMatrixLoadTransposefEXT = (PFNGLMATRIXLOADTRANSPOSEFEXTPROC) this->GetFunctionAddress("glMatrixLoadTransposefEXT");
    this->_gl->glMatrixLoadTransposedEXT = (PFNGLMATRIXLOADTRANSPOSEDEXTPROC) this->GetFunctionAddress("glMatrixLoadTransposedEXT");
    this->_gl->glMatrixMultTransposefEXT = (PFNGLMATRIXMULTTRANSPOSEFEXTPROC) this->GetFunctionAddress("glMatrixMultTransposefEXT");
    this->_gl->glMatrixMultTransposedEXT = (PFNGLMATRIXMULTTRANSPOSEDEXTPROC) this->GetFunctionAddress("glMatrixMultTransposedEXT");
    this->_gl->glNamedBufferDataEXT = (PFNGLNAMEDBUFFERDATAEXTPROC) this->GetFunctionAddress("glNamedBufferDataEXT");
    this->_gl->glNamedBufferSubDataEXT = (PFNGLNAMEDBUFFERSUBDATAEXTPROC) this->GetFunctionAddress("glNamedBufferSubDataEXT");
    this->_gl->glMapNamedBufferEXT = (PFNGLMAPNAMEDBUFFEREXTPROC) this->GetFunctionAddress("glMapNamedBufferEXT");
    this->_gl->glUnmapNamedBufferEXT = (PFNGLUNMAPNAMEDBUFFEREXTPROC) this->GetFunctionAddress("glUnmapNamedBufferEXT");
    this->_gl->glGetNamedBufferParameterivEXT = (PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetNamedBufferParameterivEXT");
    this->_gl->glGetNamedBufferPointervEXT = (PFNGLGETNAMEDBUFFERPOINTERVEXTPROC) this->GetFunctionAddress("glGetNamedBufferPointervEXT");
    this->_gl->glGetNamedBufferSubDataEXT = (PFNGLGETNAMEDBUFFERSUBDATAEXTPROC) this->GetFunctionAddress("glGetNamedBufferSubDataEXT");
    this->_gl->glProgramUniform1fEXT = (PFNGLPROGRAMUNIFORM1FEXTPROC) this->GetFunctionAddress("glProgramUniform1fEXT");
    this->_gl->glProgramUniform2fEXT = (PFNGLPROGRAMUNIFORM2FEXTPROC) this->GetFunctionAddress("glProgramUniform2fEXT");
    this->_gl->glProgramUniform3fEXT = (PFNGLPROGRAMUNIFORM3FEXTPROC) this->GetFunctionAddress("glProgramUniform3fEXT");
    this->_gl->glProgramUniform4fEXT = (PFNGLPROGRAMUNIFORM4FEXTPROC) this->GetFunctionAddress("glProgramUniform4fEXT");
    this->_gl->glProgramUniform1iEXT = (PFNGLPROGRAMUNIFORM1IEXTPROC) this->GetFunctionAddress("glProgramUniform1iEXT");
    this->_gl->glProgramUniform2iEXT = (PFNGLPROGRAMUNIFORM2IEXTPROC) this->GetFunctionAddress("glProgramUniform2iEXT");
    this->_gl->glProgramUniform3iEXT = (PFNGLPROGRAMUNIFORM3IEXTPROC) this->GetFunctionAddress("glProgramUniform3iEXT");
    this->_gl->glProgramUniform4iEXT = (PFNGLPROGRAMUNIFORM4IEXTPROC) this->GetFunctionAddress("glProgramUniform4iEXT");
    this->_gl->glProgramUniform1fvEXT = (PFNGLPROGRAMUNIFORM1FVEXTPROC) this->GetFunctionAddress("glProgramUniform1fvEXT");
    this->_gl->glProgramUniform2fvEXT = (PFNGLPROGRAMUNIFORM2FVEXTPROC) this->GetFunctionAddress("glProgramUniform2fvEXT");
    this->_gl->glProgramUniform3fvEXT = (PFNGLPROGRAMUNIFORM3FVEXTPROC) this->GetFunctionAddress("glProgramUniform3fvEXT");
    this->_gl->glProgramUniform4fvEXT = (PFNGLPROGRAMUNIFORM4FVEXTPROC) this->GetFunctionAddress("glProgramUniform4fvEXT");
    this->_gl->glProgramUniform1ivEXT = (PFNGLPROGRAMUNIFORM1IVEXTPROC) this->GetFunctionAddress("glProgramUniform1ivEXT");
    this->_gl->glProgramUniform2ivEXT = (PFNGLPROGRAMUNIFORM2IVEXTPROC) this->GetFunctionAddress("glProgramUniform2ivEXT");
    this->_gl->glProgramUniform3ivEXT = (PFNGLPROGRAMUNIFORM3IVEXTPROC) this->GetFunctionAddress("glProgramUniform3ivEXT");
    this->_gl->glProgramUniform4ivEXT = (PFNGLPROGRAMUNIFORM4IVEXTPROC) this->GetFunctionAddress("glProgramUniform4ivEXT");
    this->_gl->glProgramUniformMatrix2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix2fvEXT");
    this->_gl->glProgramUniformMatrix3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix3fvEXT");
    this->_gl->glProgramUniformMatrix4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix4fvEXT");
    this->_gl->glProgramUniformMatrix2x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix2x3fvEXT");
    this->_gl->glProgramUniformMatrix3x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix3x2fvEXT");
    this->_gl->glProgramUniformMatrix2x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix2x4fvEXT");
    this->_gl->glProgramUniformMatrix4x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix4x2fvEXT");
    this->_gl->glProgramUniformMatrix3x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix3x4fvEXT");
    this->_gl->glProgramUniformMatrix4x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix4x3fvEXT");
    this->_gl->glTextureBufferEXT = (PFNGLTEXTUREBUFFEREXTPROC) this->GetFunctionAddress("glTextureBufferEXT");
    this->_gl->glMultiTexBufferEXT = (PFNGLMULTITEXBUFFEREXTPROC) this->GetFunctionAddress("glMultiTexBufferEXT");
    this->_gl->glTextureParameterIivEXT = (PFNGLTEXTUREPARAMETERIIVEXTPROC) this->GetFunctionAddress("glTextureParameterIivEXT");
    this->_gl->glTextureParameterIuivEXT = (PFNGLTEXTUREPARAMETERIUIVEXTPROC) this->GetFunctionAddress("glTextureParameterIuivEXT");
    this->_gl->glGetTextureParameterIivEXT = (PFNGLGETTEXTUREPARAMETERIIVEXTPROC) this->GetFunctionAddress("glGetTextureParameterIivEXT");
    this->_gl->glGetTextureParameterIuivEXT = (PFNGLGETTEXTUREPARAMETERIUIVEXTPROC) this->GetFunctionAddress("glGetTextureParameterIuivEXT");
    this->_gl->glMultiTexParameterIivEXT = (PFNGLMULTITEXPARAMETERIIVEXTPROC) this->GetFunctionAddress("glMultiTexParameterIivEXT");
    this->_gl->glMultiTexParameterIuivEXT = (PFNGLMULTITEXPARAMETERIUIVEXTPROC) this->GetFunctionAddress("glMultiTexParameterIuivEXT");
    this->_gl->glGetMultiTexParameterIivEXT = (PFNGLGETMULTITEXPARAMETERIIVEXTPROC) this->GetFunctionAddress("glGetMultiTexParameterIivEXT");
    this->_gl->glGetMultiTexParameterIuivEXT = (PFNGLGETMULTITEXPARAMETERIUIVEXTPROC) this->GetFunctionAddress("glGetMultiTexParameterIuivEXT");
    this->_gl->glProgramUniform1uiEXT = (PFNGLPROGRAMUNIFORM1UIEXTPROC) this->GetFunctionAddress("glProgramUniform1uiEXT");
    this->_gl->glProgramUniform2uiEXT = (PFNGLPROGRAMUNIFORM2UIEXTPROC) this->GetFunctionAddress("glProgramUniform2uiEXT");
    this->_gl->glProgramUniform3uiEXT = (PFNGLPROGRAMUNIFORM3UIEXTPROC) this->GetFunctionAddress("glProgramUniform3uiEXT");
    this->_gl->glProgramUniform4uiEXT = (PFNGLPROGRAMUNIFORM4UIEXTPROC) this->GetFunctionAddress("glProgramUniform4uiEXT");
    this->_gl->glProgramUniform1uivEXT = (PFNGLPROGRAMUNIFORM1UIVEXTPROC) this->GetFunctionAddress("glProgramUniform1uivEXT");
    this->_gl->glProgramUniform2uivEXT = (PFNGLPROGRAMUNIFORM2UIVEXTPROC) this->GetFunctionAddress("glProgramUniform2uivEXT");
    this->_gl->glProgramUniform3uivEXT = (PFNGLPROGRAMUNIFORM3UIVEXTPROC) this->GetFunctionAddress("glProgramUniform3uivEXT");
    this->_gl->glProgramUniform4uivEXT = (PFNGLPROGRAMUNIFORM4UIVEXTPROC) this->GetFunctionAddress("glProgramUniform4uivEXT");
    this->_gl->glNamedProgramLocalParameters4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameters4fvEXT");
    this->_gl->glNamedProgramLocalParameterI4iEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameterI4iEXT");
    this->_gl->glNamedProgramLocalParameterI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameterI4ivEXT");
    this->_gl->glNamedProgramLocalParametersI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParametersI4ivEXT");
    this->_gl->glNamedProgramLocalParameterI4uiEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameterI4uiEXT");
    this->_gl->glNamedProgramLocalParameterI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameterI4uivEXT");
    this->_gl->glNamedProgramLocalParametersI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParametersI4uivEXT");
    this->_gl->glGetNamedProgramLocalParameterIivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC) this->GetFunctionAddress("glGetNamedProgramLocalParameterIivEXT");
    this->_gl->glGetNamedProgramLocalParameterIuivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC) this->GetFunctionAddress("glGetNamedProgramLocalParameterIuivEXT");
    this->_gl->glEnableClientStateiEXT = (PFNGLENABLECLIENTSTATEIEXTPROC) this->GetFunctionAddress("glEnableClientStateiEXT");
    this->_gl->glDisableClientStateiEXT = (PFNGLDISABLECLIENTSTATEIEXTPROC) this->GetFunctionAddress("glDisableClientStateiEXT");
    this->_gl->glGetFloati_vEXT = (PFNGLGETFLOATI_VEXTPROC) this->GetFunctionAddress("glGetFloati_vEXT");
    this->_gl->glGetDoublei_vEXT = (PFNGLGETDOUBLEI_VEXTPROC) this->GetFunctionAddress("glGetDoublei_vEXT");
    this->_gl->glGetPointeri_vEXT = (PFNGLGETPOINTERI_VEXTPROC) this->GetFunctionAddress("glGetPointeri_vEXT");
    this->_gl->glNamedProgramStringEXT = (PFNGLNAMEDPROGRAMSTRINGEXTPROC) this->GetFunctionAddress("glNamedProgramStringEXT");
    this->_gl->glNamedProgramLocalParameter4dEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameter4dEXT");
    this->_gl->glNamedProgramLocalParameter4dvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameter4dvEXT");
    this->_gl->glNamedProgramLocalParameter4fEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameter4fEXT");
    this->_gl->glNamedProgramLocalParameter4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC) this->GetFunctionAddress("glNamedProgramLocalParameter4fvEXT");
    this->_gl->glGetNamedProgramLocalParameterdvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC) this->GetFunctionAddress("glGetNamedProgramLocalParameterdvEXT");
    this->_gl->glGetNamedProgramLocalParameterfvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC) this->GetFunctionAddress("glGetNamedProgramLocalParameterfvEXT");
    this->_gl->glGetNamedProgramivEXT = (PFNGLGETNAMEDPROGRAMIVEXTPROC) this->GetFunctionAddress("glGetNamedProgramivEXT");
    this->_gl->glGetNamedProgramStringEXT = (PFNGLGETNAMEDPROGRAMSTRINGEXTPROC) this->GetFunctionAddress("glGetNamedProgramStringEXT");
    this->_gl->glNamedRenderbufferStorageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC) this->GetFunctionAddress("glNamedRenderbufferStorageEXT");
    this->_gl->glGetNamedRenderbufferParameterivEXT = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetNamedRenderbufferParameterivEXT");
    this->_gl->glNamedRenderbufferStorageMultisampleEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) this->GetFunctionAddress("glNamedRenderbufferStorageMultisampleEXT");
    this->_gl->glNamedRenderbufferStorageMultisampleCoverageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC) this->GetFunctionAddress("glNamedRenderbufferStorageMultisampleCoverageEXT");
    this->_gl->glCheckNamedFramebufferStatusEXT = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC) this->GetFunctionAddress("glCheckNamedFramebufferStatusEXT");
    this->_gl->glNamedFramebufferTexture1DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC) this->GetFunctionAddress("glNamedFramebufferTexture1DEXT");
    this->_gl->glNamedFramebufferTexture2DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC) this->GetFunctionAddress("glNamedFramebufferTexture2DEXT");
    this->_gl->glNamedFramebufferTexture3DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC) this->GetFunctionAddress("glNamedFramebufferTexture3DEXT");
    this->_gl->glNamedFramebufferRenderbufferEXT = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC) this->GetFunctionAddress("glNamedFramebufferRenderbufferEXT");
    this->_gl->glGetNamedFramebufferAttachmentParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetNamedFramebufferAttachmentParameterivEXT");
    this->_gl->glGenerateTextureMipmapEXT = (PFNGLGENERATETEXTUREMIPMAPEXTPROC) this->GetFunctionAddress("glGenerateTextureMipmapEXT");
    this->_gl->glGenerateMultiTexMipmapEXT = (PFNGLGENERATEMULTITEXMIPMAPEXTPROC) this->GetFunctionAddress("glGenerateMultiTexMipmapEXT");
    this->_gl->glFramebufferDrawBufferEXT = (PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC) this->GetFunctionAddress("glFramebufferDrawBufferEXT");
    this->_gl->glFramebufferDrawBuffersEXT = (PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC) this->GetFunctionAddress("glFramebufferDrawBuffersEXT");
    this->_gl->glFramebufferReadBufferEXT = (PFNGLFRAMEBUFFERREADBUFFEREXTPROC) this->GetFunctionAddress("glFramebufferReadBufferEXT");
    this->_gl->glGetFramebufferParameterivEXT = (PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetFramebufferParameterivEXT");
    this->_gl->glNamedCopyBufferSubDataEXT = (PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC) this->GetFunctionAddress("glNamedCopyBufferSubDataEXT");
    this->_gl->glNamedFramebufferTextureEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC) this->GetFunctionAddress("glNamedFramebufferTextureEXT");
    this->_gl->glNamedFramebufferTextureLayerEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC) this->GetFunctionAddress("glNamedFramebufferTextureLayerEXT");
    this->_gl->glNamedFramebufferTextureFaceEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC) this->GetFunctionAddress("glNamedFramebufferTextureFaceEXT");
    this->_gl->glTextureRenderbufferEXT = (PFNGLTEXTURERENDERBUFFEREXTPROC) this->GetFunctionAddress("glTextureRenderbufferEXT");
    this->_gl->glMultiTexRenderbufferEXT = (PFNGLMULTITEXRENDERBUFFEREXTPROC) this->GetFunctionAddress("glMultiTexRenderbufferEXT");
    this->_gl->glVertexArrayVertexOffsetEXT = (PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayVertexOffsetEXT");
    this->_gl->glVertexArrayColorOffsetEXT = (PFNGLVERTEXARRAYCOLOROFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayColorOffsetEXT");
    this->_gl->glVertexArrayEdgeFlagOffsetEXT = (PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayEdgeFlagOffsetEXT");
    this->_gl->glVertexArrayIndexOffsetEXT = (PFNGLVERTEXARRAYINDEXOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayIndexOffsetEXT");
    this->_gl->glVertexArrayNormalOffsetEXT = (PFNGLVERTEXARRAYNORMALOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayNormalOffsetEXT");
    this->_gl->glVertexArrayTexCoordOffsetEXT = (PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayTexCoordOffsetEXT");
    this->_gl->glVertexArrayMultiTexCoordOffsetEXT = (PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayMultiTexCoordOffsetEXT");
    this->_gl->glVertexArrayFogCoordOffsetEXT = (PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayFogCoordOffsetEXT");
    this->_gl->glVertexArraySecondaryColorOffsetEXT = (PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC) this->GetFunctionAddress("glVertexArraySecondaryColorOffsetEXT");
    this->_gl->glVertexArrayVertexAttribOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayVertexAttribOffsetEXT");
    this->_gl->glVertexArrayVertexAttribIOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayVertexAttribIOffsetEXT");
    this->_gl->glEnableVertexArrayEXT = (PFNGLENABLEVERTEXARRAYEXTPROC) this->GetFunctionAddress("glEnableVertexArrayEXT");
    this->_gl->glDisableVertexArrayEXT = (PFNGLDISABLEVERTEXARRAYEXTPROC) this->GetFunctionAddress("glDisableVertexArrayEXT");
    this->_gl->glEnableVertexArrayAttribEXT = (PFNGLENABLEVERTEXARRAYATTRIBEXTPROC) this->GetFunctionAddress("glEnableVertexArrayAttribEXT");
    this->_gl->glDisableVertexArrayAttribEXT = (PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC) this->GetFunctionAddress("glDisableVertexArrayAttribEXT");
    this->_gl->glGetVertexArrayIntegervEXT = (PFNGLGETVERTEXARRAYINTEGERVEXTPROC) this->GetFunctionAddress("glGetVertexArrayIntegervEXT");
    this->_gl->glGetVertexArrayPointervEXT = (PFNGLGETVERTEXARRAYPOINTERVEXTPROC) this->GetFunctionAddress("glGetVertexArrayPointervEXT");
    this->_gl->glGetVertexArrayIntegeri_vEXT = (PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC) this->GetFunctionAddress("glGetVertexArrayIntegeri_vEXT");
    this->_gl->glGetVertexArrayPointeri_vEXT = (PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC) this->GetFunctionAddress("glGetVertexArrayPointeri_vEXT");
    this->_gl->glMapNamedBufferRangeEXT = (PFNGLMAPNAMEDBUFFERRANGEEXTPROC) this->GetFunctionAddress("glMapNamedBufferRangeEXT");
    this->_gl->glFlushMappedNamedBufferRangeEXT = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC) this->GetFunctionAddress("glFlushMappedNamedBufferRangeEXT");
    this->_gl->glNamedBufferStorageEXT = (PFNGLNAMEDBUFFERSTORAGEEXTPROC) this->GetFunctionAddress("glNamedBufferStorageEXT");
    this->_gl->glClearNamedBufferDataEXT = (PFNGLCLEARNAMEDBUFFERDATAEXTPROC) this->GetFunctionAddress("glClearNamedBufferDataEXT");
    this->_gl->glClearNamedBufferSubDataEXT = (PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC) this->GetFunctionAddress("glClearNamedBufferSubDataEXT");
    this->_gl->glNamedFramebufferParameteriEXT = (PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC) this->GetFunctionAddress("glNamedFramebufferParameteriEXT");
    this->_gl->glGetNamedFramebufferParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC) this->GetFunctionAddress("glGetNamedFramebufferParameterivEXT");
    this->_gl->glProgramUniform1dEXT = (PFNGLPROGRAMUNIFORM1DEXTPROC) this->GetFunctionAddress("glProgramUniform1dEXT");
    this->_gl->glProgramUniform2dEXT = (PFNGLPROGRAMUNIFORM2DEXTPROC) this->GetFunctionAddress("glProgramUniform2dEXT");
    this->_gl->glProgramUniform3dEXT = (PFNGLPROGRAMUNIFORM3DEXTPROC) this->GetFunctionAddress("glProgramUniform3dEXT");
    this->_gl->glProgramUniform4dEXT = (PFNGLPROGRAMUNIFORM4DEXTPROC) this->GetFunctionAddress("glProgramUniform4dEXT");
    this->_gl->glProgramUniform1dvEXT = (PFNGLPROGRAMUNIFORM1DVEXTPROC) this->GetFunctionAddress("glProgramUniform1dvEXT");
    this->_gl->glProgramUniform2dvEXT = (PFNGLPROGRAMUNIFORM2DVEXTPROC) this->GetFunctionAddress("glProgramUniform2dvEXT");
    this->_gl->glProgramUniform3dvEXT = (PFNGLPROGRAMUNIFORM3DVEXTPROC) this->GetFunctionAddress("glProgramUniform3dvEXT");
    this->_gl->glProgramUniform4dvEXT = (PFNGLPROGRAMUNIFORM4DVEXTPROC) this->GetFunctionAddress("glProgramUniform4dvEXT");
    this->_gl->glProgramUniformMatrix2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix2dvEXT");
    this->_gl->glProgramUniformMatrix3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix3dvEXT");
    this->_gl->glProgramUniformMatrix4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix4dvEXT");
    this->_gl->glProgramUniformMatrix2x3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix2x3dvEXT");
    this->_gl->glProgramUniformMatrix2x4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix2x4dvEXT");
    this->_gl->glProgramUniformMatrix3x2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix3x2dvEXT");
    this->_gl->glProgramUniformMatrix3x4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix3x4dvEXT");
    this->_gl->glProgramUniformMatrix4x2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix4x2dvEXT");
    this->_gl->glProgramUniformMatrix4x3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC) this->GetFunctionAddress("glProgramUniformMatrix4x3dvEXT");
    this->_gl->glTextureBufferRangeEXT = (PFNGLTEXTUREBUFFERRANGEEXTPROC) this->GetFunctionAddress("glTextureBufferRangeEXT");
    this->_gl->glTextureStorage1DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC) this->GetFunctionAddress("glTextureStorage1DEXT");
    this->_gl->glTextureStorage2DEXT = (PFNGLTEXTURESTORAGE2DEXTPROC) this->GetFunctionAddress("glTextureStorage2DEXT");
    this->_gl->glTextureStorage3DEXT = (PFNGLTEXTURESTORAGE3DEXTPROC) this->GetFunctionAddress("glTextureStorage3DEXT");
    this->_gl->glTextureStorage2DMultisampleEXT = (PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC) this->GetFunctionAddress("glTextureStorage2DMultisampleEXT");
    this->_gl->glTextureStorage3DMultisampleEXT = (PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC) this->GetFunctionAddress("glTextureStorage3DMultisampleEXT");
    this->_gl->glVertexArrayBindVertexBufferEXT = (PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC) this->GetFunctionAddress("glVertexArrayBindVertexBufferEXT");
    this->_gl->glVertexArrayVertexAttribFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC) this->GetFunctionAddress("glVertexArrayVertexAttribFormatEXT");
    this->_gl->glVertexArrayVertexAttribIFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC) this->GetFunctionAddress("glVertexArrayVertexAttribIFormatEXT");
    this->_gl->glVertexArrayVertexAttribLFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC) this->GetFunctionAddress("glVertexArrayVertexAttribLFormatEXT");
    this->_gl->glVertexArrayVertexAttribBindingEXT = (PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC) this->GetFunctionAddress("glVertexArrayVertexAttribBindingEXT");
    this->_gl->glVertexArrayVertexBindingDivisorEXT = (PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC) this->GetFunctionAddress("glVertexArrayVertexBindingDivisorEXT");
    this->_gl->glVertexArrayVertexAttribLOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC) this->GetFunctionAddress("glVertexArrayVertexAttribLOffsetEXT");
    this->_gl->glTexturePageCommitmentEXT = (PFNGLTEXTUREPAGECOMMITMENTEXTPROC) this->GetFunctionAddress("glTexturePageCommitmentEXT");
    this->_gl->glVertexArrayVertexAttribDivisorEXT = (PFNGLVERTEXARRAYVERTEXATTRIBDIVISOREXTPROC) this->GetFunctionAddress("glVertexArrayVertexAttribDivisorEXT");
    this->_gl->glDrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC) this->GetFunctionAddress("glDrawArraysInstancedEXT");
    this->_gl->glDrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC) this->GetFunctionAddress("glDrawElementsInstancedEXT");
    this->_gl->glPolygonOffsetClampEXT = (PFNGLPOLYGONOFFSETCLAMPEXTPROC) this->GetFunctionAddress("glPolygonOffsetClampEXT");
    this->_gl->glRasterSamplesEXT = (PFNGLRASTERSAMPLESEXTPROC) this->GetFunctionAddress("glRasterSamplesEXT");
    this->_gl->glUseShaderProgramEXT = (PFNGLUSESHADERPROGRAMEXTPROC) this->GetFunctionAddress("glUseShaderProgramEXT");
    this->_gl->glActiveProgramEXT = (PFNGLACTIVEPROGRAMEXTPROC) this->GetFunctionAddress("glActiveProgramEXT");
    this->_gl->glCreateShaderProgramEXT = (PFNGLCREATESHADERPROGRAMEXTPROC) this->GetFunctionAddress("glCreateShaderProgramEXT");
    this->_gl->glFramebufferFetchBarrierEXT = (PFNGLFRAMEBUFFERFETCHBARRIEREXTPROC) this->GetFunctionAddress("glFramebufferFetchBarrierEXT");
    this->_gl->glTexStorage1DEXT = (PFNGLTEXSTORAGE1DEXTPROC) this->GetFunctionAddress("glTexStorage1DEXT");
    this->_gl->glTexStorage2DEXT = (PFNGLTEXSTORAGE2DEXTPROC) this->GetFunctionAddress("glTexStorage2DEXT");
    this->_gl->glTexStorage3DEXT = (PFNGLTEXSTORAGE3DEXTPROC) this->GetFunctionAddress("glTexStorage3DEXT");
    this->_gl->glWindowRectanglesEXT = (PFNGLWINDOWRECTANGLESEXTPROC) this->GetFunctionAddress("glWindowRectanglesEXT");
    this->_gl->glApplyFramebufferAttachmentCMAAINTEL = (PFNGLAPPLYFRAMEBUFFERATTACHMENTCMAAINTELPROC) this->GetFunctionAddress("glApplyFramebufferAttachmentCMAAINTEL");
    this->_gl->glBeginPerfQueryINTEL = (PFNGLBEGINPERFQUERYINTELPROC) this->GetFunctionAddress("glBeginPerfQueryINTEL");
    this->_gl->glCreatePerfQueryINTEL = (PFNGLCREATEPERFQUERYINTELPROC) this->GetFunctionAddress("glCreatePerfQueryINTEL");
    this->_gl->glDeletePerfQueryINTEL = (PFNGLDELETEPERFQUERYINTELPROC) this->GetFunctionAddress("glDeletePerfQueryINTEL");
    this->_gl->glEndPerfQueryINTEL = (PFNGLENDPERFQUERYINTELPROC) this->GetFunctionAddress("glEndPerfQueryINTEL");
    this->_gl->glGetFirstPerfQueryIdINTEL = (PFNGLGETFIRSTPERFQUERYIDINTELPROC) this->GetFunctionAddress("glGetFirstPerfQueryIdINTEL");
    this->_gl->glGetNextPerfQueryIdINTEL = (PFNGLGETNEXTPERFQUERYIDINTELPROC) this->GetFunctionAddress("glGetNextPerfQueryIdINTEL");
    this->_gl->glGetPerfCounterInfoINTEL = (PFNGLGETPERFCOUNTERINFOINTELPROC) this->GetFunctionAddress("glGetPerfCounterInfoINTEL");
    this->_gl->glGetPerfQueryDataINTEL = (PFNGLGETPERFQUERYDATAINTELPROC) this->GetFunctionAddress("glGetPerfQueryDataINTEL");
    this->_gl->glGetPerfQueryIdByNameINTEL = (PFNGLGETPERFQUERYIDBYNAMEINTELPROC) this->GetFunctionAddress("glGetPerfQueryIdByNameINTEL");
    this->_gl->glGetPerfQueryInfoINTEL = (PFNGLGETPERFQUERYINFOINTELPROC) this->GetFunctionAddress("glGetPerfQueryInfoINTEL");
    this->_gl->glFramebufferParameteriMESA = (PFNGLFRAMEBUFFERPARAMETERIMESAPROC) this->GetFunctionAddress("glFramebufferParameteriMESA");
    this->_gl->glGetFramebufferParameterivMESA = (PFNGLGETFRAMEBUFFERPARAMETERIVMESAPROC) this->GetFunctionAddress("glGetFramebufferParameterivMESA");
    this->_gl->glMultiDrawArraysIndirectBindlessNV = (PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSNVPROC) this->GetFunctionAddress("glMultiDrawArraysIndirectBindlessNV");
    this->_gl->glMultiDrawElementsIndirectBindlessNV = (PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSNVPROC) this->GetFunctionAddress("glMultiDrawElementsIndirectBindlessNV");
    this->_gl->glMultiDrawArraysIndirectBindlessCountNV = (PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSCOUNTNVPROC) this->GetFunctionAddress("glMultiDrawArraysIndirectBindlessCountNV");
    this->_gl->glMultiDrawElementsIndirectBindlessCountNV = (PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSCOUNTNVPROC) this->GetFunctionAddress("glMultiDrawElementsIndirectBindlessCountNV");
    this->_gl->glGetTextureHandleNV = (PFNGLGETTEXTUREHANDLENVPROC) this->GetFunctionAddress("glGetTextureHandleNV");
    this->_gl->glGetTextureSamplerHandleNV = (PFNGLGETTEXTURESAMPLERHANDLENVPROC) this->GetFunctionAddress("glGetTextureSamplerHandleNV");
    this->_gl->glMakeTextureHandleResidentNV = (PFNGLMAKETEXTUREHANDLERESIDENTNVPROC) this->GetFunctionAddress("glMakeTextureHandleResidentNV");
    this->_gl->glMakeTextureHandleNonResidentNV = (PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC) this->GetFunctionAddress("glMakeTextureHandleNonResidentNV");
    this->_gl->glGetImageHandleNV = (PFNGLGETIMAGEHANDLENVPROC) this->GetFunctionAddress("glGetImageHandleNV");
    this->_gl->glMakeImageHandleResidentNV = (PFNGLMAKEIMAGEHANDLERESIDENTNVPROC) this->GetFunctionAddress("glMakeImageHandleResidentNV");
    this->_gl->glMakeImageHandleNonResidentNV = (PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC) this->GetFunctionAddress("glMakeImageHandleNonResidentNV");
    this->_gl->glUniformHandleui64NV = (PFNGLUNIFORMHANDLEUI64NVPROC) this->GetFunctionAddress("glUniformHandleui64NV");
    this->_gl->glUniformHandleui64vNV = (PFNGLUNIFORMHANDLEUI64VNVPROC) this->GetFunctionAddress("glUniformHandleui64vNV");
    this->_gl->glProgramUniformHandleui64NV = (PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC) this->GetFunctionAddress("glProgramUniformHandleui64NV");
    this->_gl->glProgramUniformHandleui64vNV = (PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC) this->GetFunctionAddress("glProgramUniformHandleui64vNV");
    this->_gl->glIsTextureHandleResidentNV = (PFNGLISTEXTUREHANDLERESIDENTNVPROC) this->GetFunctionAddress("glIsTextureHandleResidentNV");
    this->_gl->glIsImageHandleResidentNV = (PFNGLISIMAGEHANDLERESIDENTNVPROC) this->GetFunctionAddress("glIsImageHandleResidentNV");
    this->_gl->glBlendParameteriNV = (PFNGLBLENDPARAMETERINVPROC) this->GetFunctionAddress("glBlendParameteriNV");
    this->_gl->glBlendBarrierNV = (PFNGLBLENDBARRIERNVPROC) this->GetFunctionAddress("glBlendBarrierNV");
    this->_gl->glViewportPositionWScaleNV = (PFNGLVIEWPORTPOSITIONWSCALENVPROC) this->GetFunctionAddress("glViewportPositionWScaleNV");
    this->_gl->glCreateStatesNV = (PFNGLCREATESTATESNVPROC) this->GetFunctionAddress("glCreateStatesNV");
    this->_gl->glDeleteStatesNV = (PFNGLDELETESTATESNVPROC) this->GetFunctionAddress("glDeleteStatesNV");
    this->_gl->glIsStateNV = (PFNGLISSTATENVPROC) this->GetFunctionAddress("glIsStateNV");
    this->_gl->glStateCaptureNV = (PFNGLSTATECAPTURENVPROC) this->GetFunctionAddress("glStateCaptureNV");
    this->_gl->glGetCommandHeaderNV = (PFNGLGETCOMMANDHEADERNVPROC) this->GetFunctionAddress("glGetCommandHeaderNV");
    this->_gl->glGetStageIndexNV = (PFNGLGETSTAGEINDEXNVPROC) this->GetFunctionAddress("glGetStageIndexNV");
    this->_gl->glDrawCommandsNV = (PFNGLDRAWCOMMANDSNVPROC) this->GetFunctionAddress("glDrawCommandsNV");
    this->_gl->glDrawCommandsAddressNV = (PFNGLDRAWCOMMANDSADDRESSNVPROC) this->GetFunctionAddress("glDrawCommandsAddressNV");
    this->_gl->glDrawCommandsStatesNV = (PFNGLDRAWCOMMANDSSTATESNVPROC) this->GetFunctionAddress("glDrawCommandsStatesNV");
    this->_gl->glDrawCommandsStatesAddressNV = (PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC) this->GetFunctionAddress("glDrawCommandsStatesAddressNV");
    this->_gl->glCreateCommandListsNV = (PFNGLCREATECOMMANDLISTSNVPROC) this->GetFunctionAddress("glCreateCommandListsNV");
    this->_gl->glDeleteCommandListsNV = (PFNGLDELETECOMMANDLISTSNVPROC) this->GetFunctionAddress("glDeleteCommandListsNV");
    this->_gl->glIsCommandListNV = (PFNGLISCOMMANDLISTNVPROC) this->GetFunctionAddress("glIsCommandListNV");
    this->_gl->glListDrawCommandsStatesClientNV = (PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC) this->GetFunctionAddress("glListDrawCommandsStatesClientNV");
    this->_gl->glCommandListSegmentsNV = (PFNGLCOMMANDLISTSEGMENTSNVPROC) this->GetFunctionAddress("glCommandListSegmentsNV");
    this->_gl->glCompileCommandListNV = (PFNGLCOMPILECOMMANDLISTNVPROC) this->GetFunctionAddress("glCompileCommandListNV");
    this->_gl->glCallCommandListNV = (PFNGLCALLCOMMANDLISTNVPROC) this->GetFunctionAddress("glCallCommandListNV");
    this->_gl->glBeginConditionalRenderNV = (PFNGLBEGINCONDITIONALRENDERNVPROC) this->GetFunctionAddress("glBeginConditionalRenderNV");
    this->_gl->glEndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC) this->GetFunctionAddress("glEndConditionalRenderNV");
    this->_gl->glSubpixelPrecisionBiasNV = (PFNGLSUBPIXELPRECISIONBIASNVPROC) this->GetFunctionAddress("glSubpixelPrecisionBiasNV");
    this->_gl->glConservativeRasterParameterfNV = (PFNGLCONSERVATIVERASTERPARAMETERFNVPROC) this->GetFunctionAddress("glConservativeRasterParameterfNV");
    this->_gl->glConservativeRasterParameteriNV = (PFNGLCONSERVATIVERASTERPARAMETERINVPROC) this->GetFunctionAddress("glConservativeRasterParameteriNV");
    this->_gl->glDepthRangedNV = (PFNGLDEPTHRANGEDNVPROC) this->GetFunctionAddress("glDepthRangedNV");
    this->_gl->glClearDepthdNV = (PFNGLCLEARDEPTHDNVPROC) this->GetFunctionAddress("glClearDepthdNV");
    this->_gl->glDepthBoundsdNV = (PFNGLDEPTHBOUNDSDNVPROC) this->GetFunctionAddress("glDepthBoundsdNV");
    this->_gl->glDrawVkImageNV = (PFNGLDRAWVKIMAGENVPROC) this->GetFunctionAddress("glDrawVkImageNV");
    this->_gl->glGetVkProcAddrNV = (PFNGLGETVKPROCADDRNVPROC) this->GetFunctionAddress("glGetVkProcAddrNV");
    this->_gl->glWaitVkSemaphoreNV = (PFNGLWAITVKSEMAPHORENVPROC) this->GetFunctionAddress("glWaitVkSemaphoreNV");
    this->_gl->glSignalVkSemaphoreNV = (PFNGLSIGNALVKSEMAPHORENVPROC) this->GetFunctionAddress("glSignalVkSemaphoreNV");
    this->_gl->glSignalVkFenceNV = (PFNGLSIGNALVKFENCENVPROC) this->GetFunctionAddress("glSignalVkFenceNV");
    this->_gl->glFragmentCoverageColorNV = (PFNGLFRAGMENTCOVERAGECOLORNVPROC) this->GetFunctionAddress("glFragmentCoverageColorNV");
    this->_gl->glCoverageModulationTableNV = (PFNGLCOVERAGEMODULATIONTABLENVPROC) this->GetFunctionAddress("glCoverageModulationTableNV");
    this->_gl->glGetCoverageModulationTableNV = (PFNGLGETCOVERAGEMODULATIONTABLENVPROC) this->GetFunctionAddress("glGetCoverageModulationTableNV");
    this->_gl->glCoverageModulationNV = (PFNGLCOVERAGEMODULATIONNVPROC) this->GetFunctionAddress("glCoverageModulationNV");
    this->_gl->glRenderbufferStorageMultisampleCoverageNV = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC) this->GetFunctionAddress("glRenderbufferStorageMultisampleCoverageNV");
    this->_gl->glUniform1i64NV = (PFNGLUNIFORM1I64NVPROC) this->GetFunctionAddress("glUniform1i64NV");
    this->_gl->glUniform2i64NV = (PFNGLUNIFORM2I64NVPROC) this->GetFunctionAddress("glUniform2i64NV");
    this->_gl->glUniform3i64NV = (PFNGLUNIFORM3I64NVPROC) this->GetFunctionAddress("glUniform3i64NV");
    this->_gl->glUniform4i64NV = (PFNGLUNIFORM4I64NVPROC) this->GetFunctionAddress("glUniform4i64NV");
    this->_gl->glUniform1i64vNV = (PFNGLUNIFORM1I64VNVPROC) this->GetFunctionAddress("glUniform1i64vNV");
    this->_gl->glUniform2i64vNV = (PFNGLUNIFORM2I64VNVPROC) this->GetFunctionAddress("glUniform2i64vNV");
    this->_gl->glUniform3i64vNV = (PFNGLUNIFORM3I64VNVPROC) this->GetFunctionAddress("glUniform3i64vNV");
    this->_gl->glUniform4i64vNV = (PFNGLUNIFORM4I64VNVPROC) this->GetFunctionAddress("glUniform4i64vNV");
    this->_gl->glUniform1ui64NV = (PFNGLUNIFORM1UI64NVPROC) this->GetFunctionAddress("glUniform1ui64NV");
    this->_gl->glUniform2ui64NV = (PFNGLUNIFORM2UI64NVPROC) this->GetFunctionAddress("glUniform2ui64NV");
    this->_gl->glUniform3ui64NV = (PFNGLUNIFORM3UI64NVPROC) this->GetFunctionAddress("glUniform3ui64NV");
    this->_gl->glUniform4ui64NV = (PFNGLUNIFORM4UI64NVPROC) this->GetFunctionAddress("glUniform4ui64NV");
    this->_gl->glUniform1ui64vNV = (PFNGLUNIFORM1UI64VNVPROC) this->GetFunctionAddress("glUniform1ui64vNV");
    this->_gl->glUniform2ui64vNV = (PFNGLUNIFORM2UI64VNVPROC) this->GetFunctionAddress("glUniform2ui64vNV");
    this->_gl->glUniform3ui64vNV = (PFNGLUNIFORM3UI64VNVPROC) this->GetFunctionAddress("glUniform3ui64vNV");
    this->_gl->glUniform4ui64vNV = (PFNGLUNIFORM4UI64VNVPROC) this->GetFunctionAddress("glUniform4ui64vNV");
    this->_gl->glGetUniformi64vNV = (PFNGLGETUNIFORMI64VNVPROC) this->GetFunctionAddress("glGetUniformi64vNV");
    this->_gl->glProgramUniform1i64NV = (PFNGLPROGRAMUNIFORM1I64NVPROC) this->GetFunctionAddress("glProgramUniform1i64NV");
    this->_gl->glProgramUniform2i64NV = (PFNGLPROGRAMUNIFORM2I64NVPROC) this->GetFunctionAddress("glProgramUniform2i64NV");
    this->_gl->glProgramUniform3i64NV = (PFNGLPROGRAMUNIFORM3I64NVPROC) this->GetFunctionAddress("glProgramUniform3i64NV");
    this->_gl->glProgramUniform4i64NV = (PFNGLPROGRAMUNIFORM4I64NVPROC) this->GetFunctionAddress("glProgramUniform4i64NV");
    this->_gl->glProgramUniform1i64vNV = (PFNGLPROGRAMUNIFORM1I64VNVPROC) this->GetFunctionAddress("glProgramUniform1i64vNV");
    this->_gl->glProgramUniform2i64vNV = (PFNGLPROGRAMUNIFORM2I64VNVPROC) this->GetFunctionAddress("glProgramUniform2i64vNV");
    this->_gl->glProgramUniform3i64vNV = (PFNGLPROGRAMUNIFORM3I64VNVPROC) this->GetFunctionAddress("glProgramUniform3i64vNV");
    this->_gl->glProgramUniform4i64vNV = (PFNGLPROGRAMUNIFORM4I64VNVPROC) this->GetFunctionAddress("glProgramUniform4i64vNV");
    this->_gl->glProgramUniform1ui64NV = (PFNGLPROGRAMUNIFORM1UI64NVPROC) this->GetFunctionAddress("glProgramUniform1ui64NV");
    this->_gl->glProgramUniform2ui64NV = (PFNGLPROGRAMUNIFORM2UI64NVPROC) this->GetFunctionAddress("glProgramUniform2ui64NV");
    this->_gl->glProgramUniform3ui64NV = (PFNGLPROGRAMUNIFORM3UI64NVPROC) this->GetFunctionAddress("glProgramUniform3ui64NV");
    this->_gl->glProgramUniform4ui64NV = (PFNGLPROGRAMUNIFORM4UI64NVPROC) this->GetFunctionAddress("glProgramUniform4ui64NV");
    this->_gl->glProgramUniform1ui64vNV = (PFNGLPROGRAMUNIFORM1UI64VNVPROC) this->GetFunctionAddress("glProgramUniform1ui64vNV");
    this->_gl->glProgramUniform2ui64vNV = (PFNGLPROGRAMUNIFORM2UI64VNVPROC) this->GetFunctionAddress("glProgramUniform2ui64vNV");
    this->_gl->glProgramUniform3ui64vNV = (PFNGLPROGRAMUNIFORM3UI64VNVPROC) this->GetFunctionAddress("glProgramUniform3ui64vNV");
    this->_gl->glProgramUniform4ui64vNV = (PFNGLPROGRAMUNIFORM4UI64VNVPROC) this->GetFunctionAddress("glProgramUniform4ui64vNV");
    this->_gl->glGetInternalformatSampleivNV = (PFNGLGETINTERNALFORMATSAMPLEIVNVPROC) this->GetFunctionAddress("glGetInternalformatSampleivNV");
    this->_gl->glGetMemoryObjectDetachedResourcesuivNV = (PFNGLGETMEMORYOBJECTDETACHEDRESOURCESUIVNVPROC) this->GetFunctionAddress("glGetMemoryObjectDetachedResourcesuivNV");
    this->_gl->glResetMemoryObjectParameterNV = (PFNGLRESETMEMORYOBJECTPARAMETERNVPROC) this->GetFunctionAddress("glResetMemoryObjectParameterNV");
    this->_gl->glTexAttachMemoryNV = (PFNGLTEXATTACHMEMORYNVPROC) this->GetFunctionAddress("glTexAttachMemoryNV");
    this->_gl->glBufferAttachMemoryNV = (PFNGLBUFFERATTACHMEMORYNVPROC) this->GetFunctionAddress("glBufferAttachMemoryNV");
    this->_gl->glTextureAttachMemoryNV = (PFNGLTEXTUREATTACHMEMORYNVPROC) this->GetFunctionAddress("glTextureAttachMemoryNV");
    this->_gl->glNamedBufferAttachMemoryNV = (PFNGLNAMEDBUFFERATTACHMEMORYNVPROC) this->GetFunctionAddress("glNamedBufferAttachMemoryNV");
    this->_gl->glBufferPageCommitmentMemNV = (PFNGLBUFFERPAGECOMMITMENTMEMNVPROC) this->GetFunctionAddress("glBufferPageCommitmentMemNV");
    this->_gl->glTexPageCommitmentMemNV = (PFNGLTEXPAGECOMMITMENTMEMNVPROC) this->GetFunctionAddress("glTexPageCommitmentMemNV");
    this->_gl->glNamedBufferPageCommitmentMemNV = (PFNGLNAMEDBUFFERPAGECOMMITMENTMEMNVPROC) this->GetFunctionAddress("glNamedBufferPageCommitmentMemNV");
    this->_gl->glTexturePageCommitmentMemNV = (PFNGLTEXTUREPAGECOMMITMENTMEMNVPROC) this->GetFunctionAddress("glTexturePageCommitmentMemNV");
    this->_gl->glDrawMeshTasksNV = (PFNGLDRAWMESHTASKSNVPROC) this->GetFunctionAddress("glDrawMeshTasksNV");
    this->_gl->glDrawMeshTasksIndirectNV = (PFNGLDRAWMESHTASKSINDIRECTNVPROC) this->GetFunctionAddress("glDrawMeshTasksIndirectNV");
    this->_gl->glMultiDrawMeshTasksIndirectNV = (PFNGLMULTIDRAWMESHTASKSINDIRECTNVPROC) this->GetFunctionAddress("glMultiDrawMeshTasksIndirectNV");
    this->_gl->glMultiDrawMeshTasksIndirectCountNV = (PFNGLMULTIDRAWMESHTASKSINDIRECTCOUNTNVPROC) this->GetFunctionAddress("glMultiDrawMeshTasksIndirectCountNV");
    this->_gl->glGenPathsNV = (PFNGLGENPATHSNVPROC) this->GetFunctionAddress("glGenPathsNV");
    this->_gl->glDeletePathsNV = (PFNGLDELETEPATHSNVPROC) this->GetFunctionAddress("glDeletePathsNV");
    this->_gl->glIsPathNV = (PFNGLISPATHNVPROC) this->GetFunctionAddress("glIsPathNV");
    this->_gl->glPathCommandsNV = (PFNGLPATHCOMMANDSNVPROC) this->GetFunctionAddress("glPathCommandsNV");
    this->_gl->glPathCoordsNV = (PFNGLPATHCOORDSNVPROC) this->GetFunctionAddress("glPathCoordsNV");
    this->_gl->glPathSubCommandsNV = (PFNGLPATHSUBCOMMANDSNVPROC) this->GetFunctionAddress("glPathSubCommandsNV");
    this->_gl->glPathSubCoordsNV = (PFNGLPATHSUBCOORDSNVPROC) this->GetFunctionAddress("glPathSubCoordsNV");
    this->_gl->glPathStringNV = (PFNGLPATHSTRINGNVPROC) this->GetFunctionAddress("glPathStringNV");
    this->_gl->glPathGlyphsNV = (PFNGLPATHGLYPHSNVPROC) this->GetFunctionAddress("glPathGlyphsNV");
    this->_gl->glPathGlyphRangeNV = (PFNGLPATHGLYPHRANGENVPROC) this->GetFunctionAddress("glPathGlyphRangeNV");
    this->_gl->glWeightPathsNV = (PFNGLWEIGHTPATHSNVPROC) this->GetFunctionAddress("glWeightPathsNV");
    this->_gl->glCopyPathNV = (PFNGLCOPYPATHNVPROC) this->GetFunctionAddress("glCopyPathNV");
    this->_gl->glInterpolatePathsNV = (PFNGLINTERPOLATEPATHSNVPROC) this->GetFunctionAddress("glInterpolatePathsNV");
    this->_gl->glTransformPathNV = (PFNGLTRANSFORMPATHNVPROC) this->GetFunctionAddress("glTransformPathNV");
    this->_gl->glPathParameterivNV = (PFNGLPATHPARAMETERIVNVPROC) this->GetFunctionAddress("glPathParameterivNV");
    this->_gl->glPathParameteriNV = (PFNGLPATHPARAMETERINVPROC) this->GetFunctionAddress("glPathParameteriNV");
    this->_gl->glPathParameterfvNV = (PFNGLPATHPARAMETERFVNVPROC) this->GetFunctionAddress("glPathParameterfvNV");
    this->_gl->glPathParameterfNV = (PFNGLPATHPARAMETERFNVPROC) this->GetFunctionAddress("glPathParameterfNV");
    this->_gl->glPathDashArrayNV = (PFNGLPATHDASHARRAYNVPROC) this->GetFunctionAddress("glPathDashArrayNV");
    this->_gl->glPathStencilFuncNV = (PFNGLPATHSTENCILFUNCNVPROC) this->GetFunctionAddress("glPathStencilFuncNV");
    this->_gl->glPathStencilDepthOffsetNV = (PFNGLPATHSTENCILDEPTHOFFSETNVPROC) this->GetFunctionAddress("glPathStencilDepthOffsetNV");
    this->_gl->glStencilFillPathNV = (PFNGLSTENCILFILLPATHNVPROC) this->GetFunctionAddress("glStencilFillPathNV");
    this->_gl->glStencilStrokePathNV = (PFNGLSTENCILSTROKEPATHNVPROC) this->GetFunctionAddress("glStencilStrokePathNV");
    this->_gl->glStencilFillPathInstancedNV = (PFNGLSTENCILFILLPATHINSTANCEDNVPROC) this->GetFunctionAddress("glStencilFillPathInstancedNV");
    this->_gl->glStencilStrokePathInstancedNV = (PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC) this->GetFunctionAddress("glStencilStrokePathInstancedNV");
    this->_gl->glPathCoverDepthFuncNV = (PFNGLPATHCOVERDEPTHFUNCNVPROC) this->GetFunctionAddress("glPathCoverDepthFuncNV");
    this->_gl->glCoverFillPathNV = (PFNGLCOVERFILLPATHNVPROC) this->GetFunctionAddress("glCoverFillPathNV");
    this->_gl->glCoverStrokePathNV = (PFNGLCOVERSTROKEPATHNVPROC) this->GetFunctionAddress("glCoverStrokePathNV");
    this->_gl->glCoverFillPathInstancedNV = (PFNGLCOVERFILLPATHINSTANCEDNVPROC) this->GetFunctionAddress("glCoverFillPathInstancedNV");
    this->_gl->glCoverStrokePathInstancedNV = (PFNGLCOVERSTROKEPATHINSTANCEDNVPROC) this->GetFunctionAddress("glCoverStrokePathInstancedNV");
    this->_gl->glGetPathParameterivNV = (PFNGLGETPATHPARAMETERIVNVPROC) this->GetFunctionAddress("glGetPathParameterivNV");
    this->_gl->glGetPathParameterfvNV = (PFNGLGETPATHPARAMETERFVNVPROC) this->GetFunctionAddress("glGetPathParameterfvNV");
    this->_gl->glGetPathCommandsNV = (PFNGLGETPATHCOMMANDSNVPROC) this->GetFunctionAddress("glGetPathCommandsNV");
    this->_gl->glGetPathCoordsNV = (PFNGLGETPATHCOORDSNVPROC) this->GetFunctionAddress("glGetPathCoordsNV");
    this->_gl->glGetPathDashArrayNV = (PFNGLGETPATHDASHARRAYNVPROC) this->GetFunctionAddress("glGetPathDashArrayNV");
    this->_gl->glGetPathMetricsNV = (PFNGLGETPATHMETRICSNVPROC) this->GetFunctionAddress("glGetPathMetricsNV");
    this->_gl->glGetPathMetricRangeNV = (PFNGLGETPATHMETRICRANGENVPROC) this->GetFunctionAddress("glGetPathMetricRangeNV");
    this->_gl->glGetPathSpacingNV = (PFNGLGETPATHSPACINGNVPROC) this->GetFunctionAddress("glGetPathSpacingNV");
    this->_gl->glIsPointInFillPathNV = (PFNGLISPOINTINFILLPATHNVPROC) this->GetFunctionAddress("glIsPointInFillPathNV");
    this->_gl->glIsPointInStrokePathNV = (PFNGLISPOINTINSTROKEPATHNVPROC) this->GetFunctionAddress("glIsPointInStrokePathNV");
    this->_gl->glGetPathLengthNV = (PFNGLGETPATHLENGTHNVPROC) this->GetFunctionAddress("glGetPathLengthNV");
    this->_gl->glPointAlongPathNV = (PFNGLPOINTALONGPATHNVPROC) this->GetFunctionAddress("glPointAlongPathNV");
    this->_gl->glMatrixLoad3x2fNV = (PFNGLMATRIXLOAD3X2FNVPROC) this->GetFunctionAddress("glMatrixLoad3x2fNV");
    this->_gl->glMatrixLoad3x3fNV = (PFNGLMATRIXLOAD3X3FNVPROC) this->GetFunctionAddress("glMatrixLoad3x3fNV");
    this->_gl->glMatrixLoadTranspose3x3fNV = (PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC) this->GetFunctionAddress("glMatrixLoadTranspose3x3fNV");
    this->_gl->glMatrixMult3x2fNV = (PFNGLMATRIXMULT3X2FNVPROC) this->GetFunctionAddress("glMatrixMult3x2fNV");
    this->_gl->glMatrixMult3x3fNV = (PFNGLMATRIXMULT3X3FNVPROC) this->GetFunctionAddress("glMatrixMult3x3fNV");
    this->_gl->glMatrixMultTranspose3x3fNV = (PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC) this->GetFunctionAddress("glMatrixMultTranspose3x3fNV");
    this->_gl->glStencilThenCoverFillPathNV = (PFNGLSTENCILTHENCOVERFILLPATHNVPROC) this->GetFunctionAddress("glStencilThenCoverFillPathNV");
    this->_gl->glStencilThenCoverStrokePathNV = (PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC) this->GetFunctionAddress("glStencilThenCoverStrokePathNV");
    this->_gl->glStencilThenCoverFillPathInstancedNV = (PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC) this->GetFunctionAddress("glStencilThenCoverFillPathInstancedNV");
    this->_gl->glStencilThenCoverStrokePathInstancedNV = (PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC) this->GetFunctionAddress("glStencilThenCoverStrokePathInstancedNV");
    this->_gl->glPathGlyphIndexRangeNV = (PFNGLPATHGLYPHINDEXRANGENVPROC) this->GetFunctionAddress("glPathGlyphIndexRangeNV");
    this->_gl->glPathGlyphIndexArrayNV = (PFNGLPATHGLYPHINDEXARRAYNVPROC) this->GetFunctionAddress("glPathGlyphIndexArrayNV");
    this->_gl->glPathMemoryGlyphIndexArrayNV = (PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC) this->GetFunctionAddress("glPathMemoryGlyphIndexArrayNV");
    this->_gl->glProgramPathFragmentInputGenNV = (PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC) this->GetFunctionAddress("glProgramPathFragmentInputGenNV");
    this->_gl->glGetProgramResourcefvNV = (PFNGLGETPROGRAMRESOURCEFVNVPROC) this->GetFunctionAddress("glGetProgramResourcefvNV");
    this->_gl->glFramebufferSampleLocationsfvNV = (PFNGLFRAMEBUFFERSAMPLELOCATIONSFVNVPROC) this->GetFunctionAddress("glFramebufferSampleLocationsfvNV");
    this->_gl->glNamedFramebufferSampleLocationsfvNV = (PFNGLNAMEDFRAMEBUFFERSAMPLELOCATIONSFVNVPROC) this->GetFunctionAddress("glNamedFramebufferSampleLocationsfvNV");
    this->_gl->glResolveDepthValuesNV = (PFNGLRESOLVEDEPTHVALUESNVPROC) this->GetFunctionAddress("glResolveDepthValuesNV");
    this->_gl->glScissorExclusiveNV = (PFNGLSCISSOREXCLUSIVENVPROC) this->GetFunctionAddress("glScissorExclusiveNV");
    this->_gl->glScissorExclusiveArrayvNV = (PFNGLSCISSOREXCLUSIVEARRAYVNVPROC) this->GetFunctionAddress("glScissorExclusiveArrayvNV");
    this->_gl->glMakeBufferResidentNV = (PFNGLMAKEBUFFERRESIDENTNVPROC) this->GetFunctionAddress("glMakeBufferResidentNV");
    this->_gl->glMakeBufferNonResidentNV = (PFNGLMAKEBUFFERNONRESIDENTNVPROC) this->GetFunctionAddress("glMakeBufferNonResidentNV");
    this->_gl->glIsBufferResidentNV = (PFNGLISBUFFERRESIDENTNVPROC) this->GetFunctionAddress("glIsBufferResidentNV");
    this->_gl->glMakeNamedBufferResidentNV = (PFNGLMAKENAMEDBUFFERRESIDENTNVPROC) this->GetFunctionAddress("glMakeNamedBufferResidentNV");
    this->_gl->glMakeNamedBufferNonResidentNV = (PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC) this->GetFunctionAddress("glMakeNamedBufferNonResidentNV");
    this->_gl->glIsNamedBufferResidentNV = (PFNGLISNAMEDBUFFERRESIDENTNVPROC) this->GetFunctionAddress("glIsNamedBufferResidentNV");
    this->_gl->glGetBufferParameterui64vNV = (PFNGLGETBUFFERPARAMETERUI64VNVPROC) this->GetFunctionAddress("glGetBufferParameterui64vNV");
    this->_gl->glGetNamedBufferParameterui64vNV = (PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC) this->GetFunctionAddress("glGetNamedBufferParameterui64vNV");
    this->_gl->glGetIntegerui64vNV = (PFNGLGETINTEGERUI64VNVPROC) this->GetFunctionAddress("glGetIntegerui64vNV");
    this->_gl->glUniformui64NV = (PFNGLUNIFORMUI64NVPROC) this->GetFunctionAddress("glUniformui64NV");
    this->_gl->glUniformui64vNV = (PFNGLUNIFORMUI64VNVPROC) this->GetFunctionAddress("glUniformui64vNV");
    this->_gl->glGetUniformui64vNV = (PFNGLGETUNIFORMUI64VNVPROC) this->GetFunctionAddress("glGetUniformui64vNV");
    this->_gl->glProgramUniformui64NV = (PFNGLPROGRAMUNIFORMUI64NVPROC) this->GetFunctionAddress("glProgramUniformui64NV");
    this->_gl->glProgramUniformui64vNV = (PFNGLPROGRAMUNIFORMUI64VNVPROC) this->GetFunctionAddress("glProgramUniformui64vNV");
    this->_gl->glBindShadingRateImageNV = (PFNGLBINDSHADINGRATEIMAGENVPROC) this->GetFunctionAddress("glBindShadingRateImageNV");
    this->_gl->glGetShadingRateImagePaletteNV = (PFNGLGETSHADINGRATEIMAGEPALETTENVPROC) this->GetFunctionAddress("glGetShadingRateImagePaletteNV");
    this->_gl->glGetShadingRateSampleLocationivNV = (PFNGLGETSHADINGRATESAMPLELOCATIONIVNVPROC) this->GetFunctionAddress("glGetShadingRateSampleLocationivNV");
    this->_gl->glShadingRateImageBarrierNV = (PFNGLSHADINGRATEIMAGEBARRIERNVPROC) this->GetFunctionAddress("glShadingRateImageBarrierNV");
    this->_gl->glShadingRateImagePaletteNV = (PFNGLSHADINGRATEIMAGEPALETTENVPROC) this->GetFunctionAddress("glShadingRateImagePaletteNV");
    this->_gl->glShadingRateSampleOrderNV = (PFNGLSHADINGRATESAMPLEORDERNVPROC) this->GetFunctionAddress("glShadingRateSampleOrderNV");
    this->_gl->glShadingRateSampleOrderCustomNV = (PFNGLSHADINGRATESAMPLEORDERCUSTOMNVPROC) this->GetFunctionAddress("glShadingRateSampleOrderCustomNV");
    this->_gl->glTextureBarrierNV = (PFNGLTEXTUREBARRIERNVPROC) this->GetFunctionAddress("glTextureBarrierNV");
    this->_gl->glVertexAttribL1i64NV = (PFNGLVERTEXATTRIBL1I64NVPROC) this->GetFunctionAddress("glVertexAttribL1i64NV");
    this->_gl->glVertexAttribL2i64NV = (PFNGLVERTEXATTRIBL2I64NVPROC) this->GetFunctionAddress("glVertexAttribL2i64NV");
    this->_gl->glVertexAttribL3i64NV = (PFNGLVERTEXATTRIBL3I64NVPROC) this->GetFunctionAddress("glVertexAttribL3i64NV");
    this->_gl->glVertexAttribL4i64NV = (PFNGLVERTEXATTRIBL4I64NVPROC) this->GetFunctionAddress("glVertexAttribL4i64NV");
    this->_gl->glVertexAttribL1i64vNV = (PFNGLVERTEXATTRIBL1I64VNVPROC) this->GetFunctionAddress("glVertexAttribL1i64vNV");
    this->_gl->glVertexAttribL2i64vNV = (PFNGLVERTEXATTRIBL2I64VNVPROC) this->GetFunctionAddress("glVertexAttribL2i64vNV");
    this->_gl->glVertexAttribL3i64vNV = (PFNGLVERTEXATTRIBL3I64VNVPROC) this->GetFunctionAddress("glVertexAttribL3i64vNV");
    this->_gl->glVertexAttribL4i64vNV = (PFNGLVERTEXATTRIBL4I64VNVPROC) this->GetFunctionAddress("glVertexAttribL4i64vNV");
    this->_gl->glVertexAttribL1ui64NV = (PFNGLVERTEXATTRIBL1UI64NVPROC) this->GetFunctionAddress("glVertexAttribL1ui64NV");
    this->_gl->glVertexAttribL2ui64NV = (PFNGLVERTEXATTRIBL2UI64NVPROC) this->GetFunctionAddress("glVertexAttribL2ui64NV");
    this->_gl->glVertexAttribL3ui64NV = (PFNGLVERTEXATTRIBL3UI64NVPROC) this->GetFunctionAddress("glVertexAttribL3ui64NV");
    this->_gl->glVertexAttribL4ui64NV = (PFNGLVERTEXATTRIBL4UI64NVPROC) this->GetFunctionAddress("glVertexAttribL4ui64NV");
    this->_gl->glVertexAttribL1ui64vNV = (PFNGLVERTEXATTRIBL1UI64VNVPROC) this->GetFunctionAddress("glVertexAttribL1ui64vNV");
    this->_gl->glVertexAttribL2ui64vNV = (PFNGLVERTEXATTRIBL2UI64VNVPROC) this->GetFunctionAddress("glVertexAttribL2ui64vNV");
    this->_gl->glVertexAttribL3ui64vNV = (PFNGLVERTEXATTRIBL3UI64VNVPROC) this->GetFunctionAddress("glVertexAttribL3ui64vNV");
    this->_gl->glVertexAttribL4ui64vNV = (PFNGLVERTEXATTRIBL4UI64VNVPROC) this->GetFunctionAddress("glVertexAttribL4ui64vNV");
    this->_gl->glGetVertexAttribLi64vNV = (PFNGLGETVERTEXATTRIBLI64VNVPROC) this->GetFunctionAddress("glGetVertexAttribLi64vNV");
    this->_gl->glGetVertexAttribLui64vNV = (PFNGLGETVERTEXATTRIBLUI64VNVPROC) this->GetFunctionAddress("glGetVertexAttribLui64vNV");
    this->_gl->glVertexAttribLFormatNV = (PFNGLVERTEXATTRIBLFORMATNVPROC) this->GetFunctionAddress("glVertexAttribLFormatNV");
    this->_gl->glBufferAddressRangeNV = (PFNGLBUFFERADDRESSRANGENVPROC) this->GetFunctionAddress("glBufferAddressRangeNV");
    this->_gl->glVertexFormatNV = (PFNGLVERTEXFORMATNVPROC) this->GetFunctionAddress("glVertexFormatNV");
    this->_gl->glNormalFormatNV = (PFNGLNORMALFORMATNVPROC) this->GetFunctionAddress("glNormalFormatNV");
    this->_gl->glColorFormatNV = (PFNGLCOLORFORMATNVPROC) this->GetFunctionAddress("glColorFormatNV");
    this->_gl->glIndexFormatNV = (PFNGLINDEXFORMATNVPROC) this->GetFunctionAddress("glIndexFormatNV");
    this->_gl->glTexCoordFormatNV = (PFNGLTEXCOORDFORMATNVPROC) this->GetFunctionAddress("glTexCoordFormatNV");
    this->_gl->glEdgeFlagFormatNV = (PFNGLEDGEFLAGFORMATNVPROC) this->GetFunctionAddress("glEdgeFlagFormatNV");
    this->_gl->glSecondaryColorFormatNV = (PFNGLSECONDARYCOLORFORMATNVPROC) this->GetFunctionAddress("glSecondaryColorFormatNV");
    this->_gl->glFogCoordFormatNV = (PFNGLFOGCOORDFORMATNVPROC) this->GetFunctionAddress("glFogCoordFormatNV");
    this->_gl->glVertexAttribFormatNV = (PFNGLVERTEXATTRIBFORMATNVPROC) this->GetFunctionAddress("glVertexAttribFormatNV");
    this->_gl->glVertexAttribIFormatNV = (PFNGLVERTEXATTRIBIFORMATNVPROC) this->GetFunctionAddress("glVertexAttribIFormatNV");
    this->_gl->glGetIntegerui64i_vNV = (PFNGLGETINTEGERUI64I_VNVPROC) this->GetFunctionAddress("glGetIntegerui64i_vNV");
    this->_gl->glViewportSwizzleNV = (PFNGLVIEWPORTSWIZZLENVPROC) this->GetFunctionAddress("glViewportSwizzleNV");
    this->_gl->glFramebufferTextureMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC) this->GetFunctionAddress("glFramebufferTextureMultiviewOVR");

    return True;
}
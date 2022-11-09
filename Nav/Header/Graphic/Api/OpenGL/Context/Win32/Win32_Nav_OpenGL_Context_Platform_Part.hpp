#ifndef __WIN32_NAV_OPENGL_CONTEXT_PLATFORM_PART_HPP__
#define __WIN32_NAV_OPENGL_CONTEXT_PLATFORM_PART_HPP__

#include "../Nav_OpenGL_Context.hpp"
#include <Graphic/Api/OpenGL/GL/wglext.h>
#include <Library/Nav_Library.hpp>

IMPLEMENT_PLATFORM_PART(OpenGLContext)
{
    Library* glLib;
    HDC hDc;
    HGLRC hGlRc;
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PIXELFORMATDESCRIPTOR pxFmtDesc;
};

#endif
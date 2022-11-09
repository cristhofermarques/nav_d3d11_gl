#ifndef __NAV_OPENGL_MESH_HPP__
#define __NAV_OPENGL_MESH_HPP__

#include "Graphic/Resource/Nav_Mesh.hpp"
#include "../Context/Nav_OpenGL_Context.hpp"

struct OpenGLMesh
{
    UInt32 vao;
    UInt32 vbo;
    UInt32 ibo;
    Mesh::Layout layout;
};

#endif
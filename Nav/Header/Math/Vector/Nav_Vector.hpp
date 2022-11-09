#ifndef __NAV_VECTOR_HPP__
#define __NAV_VECTOR_HPP__

#ifdef NAV_BUILD_VECTOR_DOUBLE_PRECISION

#include "Double/Nav_Double2.hpp"
#include "Double/Nav_Double3.hpp"
#include "Double/Nav_Double4.hpp"

typedef Double2 Vector2;
typedef Double3 Vector3;
typedef Double4 Vector4;

#else

#include "Single/Nav_Float2.hpp"
#include "Single/Nav_Float3.hpp"
#include "Single/Nav_Float4.hpp"

typedef Float2 Vector2;
typedef Float3 Vector3;
typedef Float4 Vector4;

#endif

#endif
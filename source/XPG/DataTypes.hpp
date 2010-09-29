#ifndef XPGH_DATATYPES
#define XPGH_DATATYPES

#include <stdint.h>

typedef int_fast8_t   int8s;
typedef int_least16_t int16s;
typedef int_least32_t int32s;
typedef int_least64_t int64s;

typedef uint8_t  int8u;
typedef uint16_t int16u;
typedef uint32_t int32u;
typedef uint64_t int64u;

#include "Matrix3D.hpp"

typedef XPG::MatrixNxN<2, float> mat2f;
typedef XPG::MatrixNxN<3, float> mat3f;
typedef XPG::MatrixNxN<4, float> mat4f;

typedef XPG::MatrixNxN<2, double> mat2d;
typedef XPG::MatrixNxN<3, double> mat3d;
typedef XPG::MatrixNxN<4, double> mat4d;

#include "Vector3D.hpp"

typedef XPG::Vector2<float> vec2f;
typedef XPG::Vector3<float> vec3f;
typedef XPG::Vector4<float> vec4f;

typedef XPG::Vector2<double> vec2d;
typedef XPG::Vector3<double> vec3d;
typedef XPG::Vector4<double> vec4d;

#endif

#ifndef XPGH_OPENGL
#define XPGH_OPENGL

#include "Platforms.hpp"

#ifdef XPG_PLATFORM_ANDROID
#   define XPG_OPENGL_ES
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
    const GLenum OGLIT = GL_UNSIGNED_SHORT; // OpenGL Index Type
    typedef GLushort OGLI; // abstraction of an "OpenGL Index"
    typedef char GLchar; // small fix to keep tools consistent
#else
#   include <GL/glew.h>
    const GLenum OGLIT = GL_UNSIGNED_INT;
    typedef GLuint OGLI;
#endif

#endif

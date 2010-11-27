#ifndef XPGH_OPENGL
#define XPGH_OPENGL

#include "OperatingSystems.hpp"

#ifdef XPG_OS_ANDROID
#   define XPG_OPENGL_ES
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#else
#   include <GL/glew.h>
#endif

#endif

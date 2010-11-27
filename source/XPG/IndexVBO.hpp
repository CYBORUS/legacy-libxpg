#ifndef XPGH_INDEXVBO
#define XPGH_INDEXVBO

#include "OpenGL.hpp"

#ifdef XPG_OPENGL_ES
#   define OGL_INDEX GLushort
#else
#   define OGL_INDEX GLuint
#endif

namespace XPG
{
    class IndexVBO
    {
        public:
            IndexVBO();
            ~IndexVBO();

            void draw() const;
            void loadData(GLenum inMode, GLuint inSize, const OGL_INDEX* inData,
                GLenum inUsage = GL_STATIC_DRAW);
            void setMode(GLenum inMode);

        private:
            GLenum mMode;
            GLuint mBuffer;
            GLuint mSize;
    };
}

#endif


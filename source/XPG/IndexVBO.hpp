#ifndef XPGH_INDEXVBO
#define XPGH_INDEXVBO

#include <GL/glew.h>

namespace XPG
{
    class IndexVBO
    {
        public:
            IndexVBO();
            ~IndexVBO();

            void draw(GLenum inMode = GL_TRIANGLES) const;
            void loadData(GLuint inSize, const GLuint* inData,
                GLenum inUsage = GL_STATIC_DRAW);

        private:
            GLuint mBuffer;
            GLuint mSize;
    };
}

#endif


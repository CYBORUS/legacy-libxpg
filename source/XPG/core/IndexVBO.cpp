#include "../IndexVBO.hpp"

namespace XPG
{
    IndexVBO::IndexVBO() : mSize(0)
    {
        glGenBuffers(1, &mBuffer);
    }

    IndexVBO::~IndexVBO()
    {
        glDeleteBuffers(1, &mBuffer);
    }

    void IndexVBO::loadData(GLenum inMode, GLuint inSize,
        const OGL_INDEX* inData, GLenum inUsage)
    {
        mMode = inMode;
        mSize = inSize;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, inSize * sizeof(OGL_INDEX),
            inData, inUsage);
    }

    void IndexVBO::setMode(GLenum inMode)
    {
        mMode = inMode;
    }

    void IndexVBO::draw() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer);
        glDrawElements(mMode, mSize,
#ifdef XPG_OPENGL_ES
            GL_UNSIGNED_SHORT,
#else
            GL_UNSIGNED_INT,
#endif
            0);
    }
}

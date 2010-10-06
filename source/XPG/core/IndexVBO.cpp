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

    void IndexVBO::loadData(GLuint inSize, const GLuint* inData, GLenum inUsage)
    {
        mSize = inSize;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, inSize * sizeof(GLuint), inData,
            inUsage);
    }

    void IndexVBO::draw(GLenum inMode) const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer);
        glDrawElements(inMode, mSize, GL_UNSIGNED_INT, 0);
    }
}

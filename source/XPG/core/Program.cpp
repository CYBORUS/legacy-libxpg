#include "../Program.hpp"

#include <iostream>

namespace XPG
{
    Program::Program() : mHandle(0), mSize(0), mLinked(false)
    {
    }

    Program::~Program()
    {
        clear();
    }

    void Program::attachShader(const Shader& inShader)
    {
        attachShader(inShader.handle());
    }

    void Program::attachShader(GLuint inShader)
    {
        if (mLinked)
        {
            // More shaders cannot be attached if the program is already linked.
            return;
        }

        if (mSize >= XPG_MAX_SHADERS)
        {
            return;
        }

        if (!mSize)
        {
            // If the shader container is empty, we have not yet created the
            // shader program. That needs to be done first!

            mHandle = glCreateProgram();
            if (!mHandle)
            {
                // TODO: report error through XPG exception
                std::cerr << "failure on glCreateProgram\n";
                return;
            }
        }

        mShaders[mSize] = inShader;
        ++mSize;

        glAttachShader(mHandle, inShader);
    }

    void Program::bindAttribLocation(GLuint inIndex, const GLchar* inName)
    {
        glBindAttribLocation(mHandle, inIndex, inName);
    }

    void Program::link()
    {
        if (mLinked)
        {
            // TODO: report error through XPG exception
            std::cerr << "program already linked\n";
            return;
        }

        if (mSize < 2 || !mHandle)
        {
            // TODO: report error through XPG exception
            std::cerr << "cannot link program -- inadequate shaders\n";
            return;
        }

        glLinkProgram(mHandle);

        GLint linked;
        glGetProgramiv(mHandle, GL_LINK_STATUS, &linked);

        if (!linked)
        {
            // TODO: report error through XPG exception
            std::cerr << "failure on glLinkProgram\n";
            return;
        }

        mLinked = true;
        use();
    }

    GLint Program::getUniformLocation(const GLchar* inName)
    {
        return glGetUniformLocation(mHandle, inName);
    }

    void Program::clear()
    {
        for (size_t i = 0; i < mSize; ++i)
            glDetachShader(mHandle, mShaders[i]);

        mSize = 0;
        mLinked = false;

        if (mHandle)
        {
            glDeleteProgram(mHandle);
            mHandle = 0;
        }
    }
}

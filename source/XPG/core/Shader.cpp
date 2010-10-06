#include "../Shader.hpp"

#include <iostream>
#include <fstream>
using namespace std;

namespace XPG
{
    Shader::Shader() : mHandle(0)
    {
    }

    Shader::Shader(const char* inFile, GLenum inType) : mHandle(0)
    {
        load(inFile, inType);
    }

    Shader::~Shader()
    {
        if (mHandle) glDeleteShader(mHandle);
    }

    void Shader::load(const char* inFile, GLenum inType)
    {
        if (mHandle || !inFile) return;

        char* source = fileToBuffer(inFile);
        if (!source)
        {
            // TODO: report error
            return;
        }

        mHandle = glCreateShader(inType);
        if (!mHandle)
        {
            // TODO: report error
            return;
        }

        glShaderSource(mHandle, 1, const_cast<const GLchar**>(&source), 0);
        glCompileShader(mHandle);

        GLint compiled;
        glGetShaderiv(mHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            // TODO: convert to XPG exception
            GLchar log[2048];
            GLsizei length;
            glGetShaderInfoLog(mHandle, 2048, &length, log);
            cerr << inFile << " -- shader compiler errors --\n" << log << '\n';
        }

        delete [] source;
    }

    char* Shader::fileToBuffer(const char* inFile)
    {
        char* outBuffer;
        ifstream fin(inFile);
        if (fin.fail()) return NULL;
        fin.seekg(0, ios::end);
        size_t length = fin.tellg();
        fin.seekg(0, ios::beg);
        outBuffer = new char[length + 1];
        fin.read(outBuffer, length);
        outBuffer[length] = 0;
        fin.close();

        return outBuffer;
    }
}

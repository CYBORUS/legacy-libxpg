#ifndef XPGH_PROGRAM
#define XPGH_PROGRAM

#include "Shader.hpp"

#define XPG_MAX_SHADERS 3

namespace XPG
{
    class Program
    {
        public:
            Program();
            ~Program();

            void attachShader(const Shader& inShader);
            void attachShader(GLuint inShader);
            void bindAttribLocation(GLuint inIndex, const GLchar* inName);
            void link();
            GLint getUniformLocation(const GLchar* inName);
            void clear();

            inline void use() { glUseProgram(mHandle); }

        private:
            GLuint mHandle;
            GLuint mShaders[XPG_MAX_SHADERS];
            size_t mSize;
            bool mLinked;
    };
}

#endif

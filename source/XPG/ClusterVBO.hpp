#ifndef XPGH_CLUSTERVBO
#define XPGH_CLUSTERVBO

namespace XPG
{
    template<size_t N>
    class ClusterVBO
    {
        public:
            ClusterVBO();
            ~ClusterVBO();

            void display(GLenum inMode, GLint inFirst, GLsizei inCount) const;
            void loadVAA(GLuint inVAI, GLuint inVPP, GLuint inSize,
                const GLfloat* inData, GLenum inUsage = GL_STATIC_DRAW);

        private:
            GLuint mVBOI[N]; // vertex buffer objects indices
            GLuint mVPV[N]; // values per vertex
    };

    template<size_t N>
    ClusterVBO<N>::ClusterVBO()
    {
        glGenBuffers(N, mVBOI);
    }

    template<size_t N>
    ClusterVBO<N>::~ClusterVBO()
    {
        glDeleteBuffers(N, mVBOI);
    }

    template<size_t N>
    void ClusterVBO<N>::display(GLenum inMode, GLint inFirst, GLsizei inCount)
        const
    {
        for (size_t i = 0; i < N; ++i)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mVBOI[i]);
            glVertexAttribPointer(i, mVPV[i], GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(i);
        }

        glDrawArrays(inMode, inFirst, inCount);

        for (size_t i = 0; i < N; ++i)
            glDisableVertexAttribArray(i);
    }

    template<size_t N>
    void ClusterVBO<N>::loadVAA(GLuint inVAI, GLuint inVPP, GLuint inSize,
        const GLfloat* inData, GLenum inUsage)
    {
        mVPV[inVAI] = inVPP;
        glBindBuffer(GL_ARRAY_BUFFER, mVBOI[inVAI]);
        glBufferData(GL_ARRAY_BUFFER, inSize * sizeof(GLfloat) * mVPV[inVAI],
            inData, inUsage);
    }
}

#endif


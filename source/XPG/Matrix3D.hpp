#ifndef XPGH_MATRIX3D
#define XPGH_MATRIX3D

#define PI 3.141592653589793238462643383
#define DEG2RAD(n) ((n) * PI / 180.0)
#define RAD2DEG(n) ((n) * 180.0 / PI)

#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>

namespace XPG
{
    template<size_t N, typename T>
    class MatrixNxN
    {
        public:
            MatrixNxN<N, T>();

            template<size_t N2>
            MatrixNxN<N, T>(const MatrixNxN<N2, T>& inMatrix)
            {
                if (N == N2)
                {
                    memcpy(MatrixNxN<N, T>::mData, inMatrix.array(),
                        N * N * sizeof(T));
                }
                else if (N > N2)
                {
                    loadIdentity();
                    for (size_t i = 0; i < N2; ++i)
                    {
                        for (size_t j = 0; j < N2; ++j)
                        {
                            (*this)(i, j) = inMatrix(i, j);
                        }
                    }
                }
                else
                {
                    for (size_t i = 0; i < N; ++i)
                    {
                        for (size_t j = 0; j < N; ++j)
                        {
                            (*this)(i, j) = inMatrix(i, j);
                        }
                    }
                }
            }

            static void prepareIdentity();

            void loadIdentity();

            void multiply(const MatrixNxN<N, T>& inMatrix);

            inline MatrixNxN<N, T>& operator=(const MatrixNxN<N, T>& inMatrix)
            {
                if (this != &inMatrix) copy(inMatrix);
            }

            inline MatrixNxN<N, T>& operator*=(const MatrixNxN<N, T>& inMatrix)
            {
                multiply(inMatrix);
                return *this;
            }

            inline T* array() { return mData; }
            inline const T* array() const { return mData; }
            inline T& operator[](size_t inIndex) { return mData[inIndex]; }
            inline T operator[](size_t inIndex) const { return mData[inIndex]; }

            inline T& operator()(size_t inRow, size_t inCol)
            {
                return mData[inCol * N + inRow];
            }

            inline T operator()(size_t inRow, size_t inCol) const
            {
                return mData[inCol * N + inRow];
            }

            inline bool operator==(const MatrixNxN& inMatrix) const
            {
                return !memcmp(mData, inMatrix.mData, (N * N) * sizeof(T));
            }

            inline bool operator!=(const MatrixNxN& inMatrix) const
            {
                return memcmp(mData, inMatrix.mData, (N * N) * sizeof(T));
            }

            inline void copy(const MatrixNxN<N, T>& inMatrix)
            {
                memcpy(mData, inMatrix.mData, (N * N) * sizeof(T));
            }

        private:
            T mData[N * N];

            static T mIdentity[N * N];
    };

    template<size_t N, typename T>
    T MatrixNxN<N, T>::mIdentity[N * N];

    template<size_t N, typename T>
    void MatrixNxN<N, T>::prepareIdentity()
    {
        for (size_t i = 0; i < (N * N); ++i)
            mIdentity[i] = i % (N + 1) ? static_cast<T>(0) : static_cast<T>(1);
    }

    template<size_t N, typename T>
    MatrixNxN<N, T>::MatrixNxN()
    {
        loadIdentity();
    }

    template<size_t N, typename T>
    void MatrixNxN<N, T>::loadIdentity()
    {
        memcpy(mData, mIdentity, (N * N) * sizeof(T));
    }

    template<size_t N, typename T>
    void MatrixNxN<N, T>::multiply(const MatrixNxN<N, T>& inMatrix)
    {
        MatrixNxN<N, T> m(*this);

        for (size_t i = 0; i < N; ++i)
        {
            for (size_t j = 0; j < N; ++j)
            {
                T value = static_cast<T>(0);
                for (size_t k = 0; k < N; ++k)
                    value += m(i, k) * inMatrix(k, j);

                //at(i, j) = value;
                mData[j * N + i] = value;
            }
        }
    }

    template<size_t N, typename T>
    std::ostream& operator<<(std::ostream& inStream,
        const MatrixNxN<N, T>& inMatrix)
    {
        inStream << std::setprecision(4);
        for (size_t i = 0; i < N; ++i)
        {
            if (i) inStream << '\n';
            for (size_t j = 0; j < N; ++j)
            {
                inStream << std::setw(8) << inMatrix(i, j);
            }
        }

        return inStream;
    }
}

#endif

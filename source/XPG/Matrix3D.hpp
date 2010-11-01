#ifndef XPGH_MATRIX3D
#define XPGH_MATRIX3D

#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>

#define PI 3.141592653589793238462643383
#define DEG2RAD(n) ((n) * PI / 180.0)
#define RAD2DEG(n) ((n) * 180.0 / PI)

#define SCT(n) static_cast<T>(n)

namespace XPG
{
    template<typename T>
    class Matrix4x4
    {
        public:
            Matrix4x4(const T* inArray = mIdentity);
            Matrix4x4(const Matrix4x4<T>& inMatrix);
            Matrix4x4(const Matrix4x4<T>& inLMatrix,
                const Matrix4x4<T>& inRMatrix);
            ~Matrix4x4();

            void loadIdentity();

            /// model view
            void rotate(T inDegrees, T inX, T inY, T inZ);
            void rotateX(T inDegrees);
            void rotateY(T inDegrees);
            void rotateZ(T inDegrees);
            void scale(T inScale);
            void scaleX(T inScale);
            void scaleY(T inScale);
            void scaleZ(T inScale);
            void scale(T inX, T inY, T inZ);
            void translate(T inX, T inY, T inZ);
            void smartMove(T inRX, T inRY, T inRZ, T inTX, T inTY, T inTZ);

            /// projection
            void frustum(T inLeft, T inRight, T inBottom, T inTop, T inNear,
                T inFar);
            void perspective(T inFieldOfView, T inRatio, T inNear, T inFar);
            void orthographic(T inLeft, T inRight, T inBottom, T inTop,
                T inNear, T inFar);
            void orthographic(T inRange, T inRatio);

            /// matrix operators
            Matrix4x4<T>& operator=(const Matrix4x4<T>& inMatrix);
            void multiply(const Matrix4x4<T>& inLMatrix,
                const Matrix4x4<T>& inRMatrix);
            void inverse();
            void copyInverseTo(Matrix4x4<T>& inMatrix) const;
            const Matrix4x4<T> inversed() const;

            inline T* array() { return mData; }
            inline const T* array() const { return mData; }
            inline T& operator[](size_t inIndex) { return mData[inIndex]; }
            inline T operator[](size_t inIndex) const { return mData[inIndex]; }

            inline T& operator()(size_t inRow, size_t inCol)
            {
                return mData[inCol * 4 + inRow];
            }

            inline T operator()(size_t inRow, size_t inCol) const
            {
                return mData[inCol * 4 + inRow];
            }

            inline bool operator==(const Matrix4x4<T>& inMatrix) const
            {
                return !memcmp(mData, inMatrix.mData, 16 * sizeof(T));
            }

            inline bool operator!=(const Matrix4x4<T>& inMatrix) const
            {
                return memcmp(mData, inMatrix.mData, 16 * sizeof(T));
            }

            inline Matrix4x4<T>& operator*=(const Matrix4x4<T>& inMatrix)
            {
                multiply(Matrix4x4<T>(mData), inMatrix);
                return *this;
            }

            inline const Matrix4x4<T> operator*(const Matrix4x4<T>& inMatrix)
                const
            {
                return Matrix4x4<T>(mData) *= inMatrix;
            }

        private:
            inline void copy(const Matrix4x4<T>& inMatrix)
            {
                memcpy(mData, inMatrix.mData, 16 * sizeof(T));
            }

            inline T& at(size_t inRow, size_t inCol)
            {
                return mData[inCol * 4 + inRow];
            }

            inline T at(size_t inRow, size_t inCol) const
            {
                return mData[inCol * 4 + inRow];
            }

            T mData[16];

            static const T mIdentity[16];
    };

    template<typename T>
    const T Matrix4x4<T>::mIdentity[16] = {
        SCT(1), SCT(0), SCT(0), SCT(0),
        SCT(0), SCT(1), SCT(0), SCT(0),
        SCT(0), SCT(0), SCT(1), SCT(0),
        SCT(0), SCT(0), SCT(0), SCT(1)
        };

    template<typename T>
    Matrix4x4<T>::Matrix4x4(const T* inArray)
    {
        memcpy(mData, inArray, 16 * sizeof(T));
    }

    template<typename T>
    Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& inMatrix)
    {
        copy(inMatrix);
    }

    template<typename T>
    Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& inLMatrix,
        const Matrix4x4<T>& inRMatrix)
    {
        multiply(inLMatrix, inRMatrix);
    }

    template<typename T>
    Matrix4x4<T>::~Matrix4x4()
    {
    }

    template<typename T>
    void Matrix4x4<T>::loadIdentity()
    {
        memcpy(mData, mIdentity, 16 * sizeof(T));
    }

    template<typename T>
    void Matrix4x4<T>::rotate(T inDegrees, T inX, T inY, T inZ)
    {
        /// arbitrary rotation about an axis
        /// http://www.opengl.org/sdk/docs/man/xhtml/glRotate.xml

        T r = DEG2RAD(inDegrees);
        T c = cos(r);
        T ci = SCT(1) - c;
        T s = sin(r);
        Matrix4x4<T> transform;

        transform[0] = inX * inX * ci + c;
        transform[4] = inX * inY * ci - (inZ * s);
        transform[8] = inX * inZ * ci + (inY * s);
        transform[1] = inY * inX * ci + (inZ * s);
        transform[5] = inY * inY * ci + c;
        transform[9] = inY * inZ * ci - (inX * s);
        transform[2] = inX * inZ * ci - (inY * s);
        transform[6] = inY * inZ * ci + (inX * s);
        transform[10] = inZ * inZ * ci + c;

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::rotateX(T inDegrees)
    {
        T r = DEG2RAD(inDegrees);
        T c = cos(r);
        T s = sin(r);
        Matrix4x4<T> transform;

        transform[5] = c;
        transform[9] = -s;
        transform[6] = s;
        transform[10] = c;

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::rotateY(T inDegrees)
    {
        T r = DEG2RAD(inDegrees);
        T c = cos(r);
        T s = sin(r);
        Matrix4x4<T> transform;

        transform[0] = c;
        transform[8] = s;
        transform[2] = -s;
        transform[10] = c;

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::rotateZ(T inDegrees)
    {
        T r = DEG2RAD(inDegrees);
        T c = cos(r);
        T s = sin(r);
        Matrix4x4<T> transform;

        transform[0] = c;
        transform[4] = -s;
        transform[1] = s;
        transform[5] = c;

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::scale(T inScale)
    {
        Matrix4x4<T> transform;

        transform[0] = inScale;
        transform[5] = inScale;
        transform[10] = inScale;

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::scaleX(T inScale)
    {
        Matrix4x4<T> transform;

        transform[0] = inScale;
        transform[5] = SCT(1);
        transform[10] = SCT(1);

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::scaleY(T inScale)
    {
        Matrix4x4<T> transform;

        transform[0] = SCT(1);
        transform[5] = inScale;
        transform[10] = SCT(1);

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::scaleZ(T inScale)
    {
        Matrix4x4<T> transform;

        transform[0] = SCT(1);
        transform[5] = SCT(1);
        transform[10] = inScale;

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::scale(T inX, T inY, T inZ)
    {
        Matrix4x4<T> transform;

        transform[0] = inX;
        transform[5] = inY;
        transform[10] = inZ;

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::translate(T inX, T inY, T inZ)
    {
        Matrix4x4<T> transform;

        transform[12] = inX;
        transform[13] = inY;
        transform[14] = inZ;

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::smartMove(T inRX, T inRY, T inRZ, T inTX, T inTY, T inTZ)
    {
        translate(inTX, inTY, inTZ);
        rotateY(inRY);
        rotateX(inRX);
        rotateZ(inRZ);
    }

    template<typename T>
    void Matrix4x4<T>::frustum(T inLeft, T inRight, T inBottom, T inTop,
        T inNear, T inFar)
    {
        Matrix4x4<T> transform;

        transform[0] = (SCT(2) * inNear) / (inRight - inLeft);
        transform[8] = (inRight + inLeft) / (inRight - inLeft);
        transform[5] = (SCT(2) * inNear) / (inTop - inBottom);
        transform[9] = (inTop + inBottom) / (inTop - inBottom);
        transform[10] = (inFar + inNear) / (inNear - inFar);
        transform[14] = (SCT(2) * inFar * inNear) / (inNear - inFar);
        transform[11] = SCT(-1);
        transform[15] = SCT(0);

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::perspective(T inFieldOfView, T inRatio, T inNear,
        T inFar)
    {
        /// adaptation of gluPerspective
        /// http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml
        T r = DEG2RAD(inFieldOfView);
        T f = SCT(1) / tan(r / SCT(2));

        Matrix4x4<T> transform;

        transform[0] = f / inRatio;
        transform[5] = f;
        transform[10] = (inFar + inNear) / (inNear - inFar);
        transform[14] = (SCT(2) * inFar * inNear) / (inNear - inFar);
        transform[11] = SCT(-1);
        transform[15] = SCT(0);

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::orthographic(T inLeft, T inRight, T inBottom, T inTop,
        T inNear, T inFar)
    {
        Matrix4x4<T> transform;

        transform[0] = SCT(2) / (inRight - inLeft);
        transform[12] = (inRight + inLeft) / (inRight - inLeft);
        transform[5] = SCT(2) / (inTop - inBottom);
        transform[13] = (inTop + inBottom) / (inTop - inBottom);
        transform[10] = SCT(2) / (inNear - inFar);
        transform[14] = (inFar + inNear) / (inFar - inNear);

        multiply(Matrix4x4<T>(mData), transform);
    }

    template<typename T>
    void Matrix4x4<T>::orthographic(T inRange, T inRatio)
    {
        if (inRatio < SCT(1))
        {
            orthographic(-inRange, inRange, -inRange / inRatio,
                inRange / inRatio, -inRange, inRange);
        }
        else
        {
            orthographic(-inRange * inRatio, inRange * inRatio, -inRange,
                inRange, -inRange, inRange);
        }
    }

    template<typename T>
    Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& inMatrix)
    {
        if (this != &inMatrix) copy(inMatrix);
        return *this;
    }

    template<typename T>
    void Matrix4x4<T>::multiply(const Matrix4x4<T>& inLMatrix,
        const Matrix4x4<T>& inRMatrix)
    {
        mData[0] = (inLMatrix[0] * inRMatrix[0])
            + (inLMatrix[4] * inRMatrix[1])
            + (inLMatrix[8] * inRMatrix[2])
            + (inLMatrix[12] * inRMatrix[3]);

        mData[4] = (inLMatrix[0] * inRMatrix[4])
            + (inLMatrix[4] * inRMatrix[5])
            + (inLMatrix[8] * inRMatrix[6])
            + (inLMatrix[12] * inRMatrix[7]);

        mData[8] = (inLMatrix[0] * inRMatrix[8])
            + (inLMatrix[4] * inRMatrix[9])
            + (inLMatrix[8] * inRMatrix[10])
            + (inLMatrix[12] * inRMatrix[11]);

        mData[12] = (inLMatrix[0] * inRMatrix[12])
            + (inLMatrix[4] * inRMatrix[13])
            + (inLMatrix[8] * inRMatrix[14])
            + (inLMatrix[12] * inRMatrix[15]);

        mData[1] = (inLMatrix[1] * inRMatrix[0])
            + (inLMatrix[5] * inRMatrix[1])
            + (inLMatrix[9] * inRMatrix[2])
            + (inLMatrix[13] * inRMatrix[3]);

        mData[5] = (inLMatrix[1] * inRMatrix[4])
            + (inLMatrix[5] * inRMatrix[5])
            + (inLMatrix[9] * inRMatrix[6])
            + (inLMatrix[13] * inRMatrix[7]);

        mData[9] = (inLMatrix[1] * inRMatrix[8])
            + (inLMatrix[5] * inRMatrix[9])
            + (inLMatrix[9] * inRMatrix[10])
            + (inLMatrix[13] * inRMatrix[11]);

        mData[13] = (inLMatrix[1] * inRMatrix[12])
            + (inLMatrix[5] * inRMatrix[13])
            + (inLMatrix[9] * inRMatrix[14])
            + (inLMatrix[13] * inRMatrix[15]);

        mData[2] = (inLMatrix[2] * inRMatrix[0])
            + (inLMatrix[6] * inRMatrix[1])
            + (inLMatrix[10] * inRMatrix[2])
            + (inLMatrix[14] * inRMatrix[3]);

        mData[6] = (inLMatrix[2] * inRMatrix[4])
            + (inLMatrix[6] * inRMatrix[5])
            + (inLMatrix[10] * inRMatrix[6])
            + (inLMatrix[14] * inRMatrix[7]);

        mData[10] = (inLMatrix[2] * inRMatrix[8])
            + (inLMatrix[6] * inRMatrix[9])
            + (inLMatrix[10] * inRMatrix[10])
            + (inLMatrix[14] * inRMatrix[11]);

        mData[14] = (inLMatrix[2] * inRMatrix[12])
            + (inLMatrix[6] * inRMatrix[13])
            + (inLMatrix[10] * inRMatrix[14])
            + (inLMatrix[14] * inRMatrix[15]);

        mData[3] = (inLMatrix[3] * inRMatrix[0])
            + (inLMatrix[7] * inRMatrix[1])
            + (inLMatrix[11] * inRMatrix[2])
            + (inLMatrix[15] * inRMatrix[3]);

        mData[7] = (inLMatrix[3] * inRMatrix[4])
            + (inLMatrix[7] * inRMatrix[5])
            + (inLMatrix[11] * inRMatrix[6])
            + (inLMatrix[15] * inRMatrix[7]);

        mData[11] = (inLMatrix[3] * inRMatrix[8])
            + (inLMatrix[7] * inRMatrix[9])
            + (inLMatrix[11] * inRMatrix[10])
            + (inLMatrix[15] * inRMatrix[11]);

        mData[15] = (inLMatrix[3] * inRMatrix[12])
            + (inLMatrix[7] * inRMatrix[13])
            + (inLMatrix[11] * inRMatrix[14])
            + (inLMatrix[15] * inRMatrix[15]);
    }

    template<typename T>
    void Matrix4x4<T>::inverse()
    {
        const Matrix4x4<T> m(*this);
        m.copyInverseTo(*this);
    }

    template<typename T>
    const Matrix4x4<T> Matrix4x4<T>::inversed() const
    {
        Matrix4x4<T> outMatrix;
        copyInverseTo(outMatrix);
        return outMatrix;
    }

    template<typename T>
    void Matrix4x4<T>::copyInverseTo(Matrix4x4<T>& inMatrix) const
    {
#define SWAP_ROWS(a, b) { T* _tmp = a; (a) = (b); (b) = _tmp; }

        T wtmp[4][8];
        T m0;
        T m1;
        T m2;
        T m3;
        T s;
        T* r0;
        T* r1;
        T* r2;
        T* r3;

        r0 = wtmp[0];
        r1 = wtmp[1];
        r2 = wtmp[2];
        r3 = wtmp[3];

        r0[0] = at(0, 0);
        r0[1] = at(0, 1);
        r0[2] = at(0, 2);
        r0[3] = at(0, 3);
        r0[4] = 1.0;
        r0[5] = r0[6] = r0[7] = 0.0;
        r1[0] = at(1, 0);
        r1[1] = at(1, 1);
        r1[2] = at(1, 2);
        r1[3] = at(1, 3);
        r1[5] = 1.0;
        r1[4] = r1[6] = r1[7] = 0.0;
        r2[0] = at(2, 0);
        r2[1] = at(2, 1);
        r2[2] = at(2, 2);
        r2[3] = at(2, 3);
        r2[6] = 1.0;
        r2[4] = r2[5] = r2[7] = 0.0;
        r3[0] = at(3, 0);
        r3[1] = at(3, 1);
        r3[2] = at(3, 2);
        r3[3] = at(3, 3);
        r3[7] = 1.0;
        r3[4] = r3[5] = r3[6] = 0.0;

        if (fabs(r3[0]) > fabs(r2[0])) SWAP_ROWS(r3, r2);
        if (fabs(r2[0]) > fabs(r1[0])) SWAP_ROWS(r2, r1);
        if (fabs(r1[0]) > fabs(r0[0])) SWAP_ROWS(r1, r0);
        if (0.0 == r0[0]) return;

        m1 = r1[0] / r0[0];
        m2 = r2[0] / r0[0];
        m3 = r3[0] / r0[0];
        s = r0[1];
        r1[1] -= m1 * s;
        r2[1] -= m2 * s;
        r3[1] -= m3 * s;
        s = r0[2];
        r1[2] -= m1 * s;
        r2[2] -= m2 * s;
        r3[2] -= m3 * s;
        s = r0[3];
        r1[3] -= m1 * s;
        r2[3] -= m2 * s;
        r3[3] -= m3 * s;
        s = r0[4];

        if (s != 0.0)
        {
            r1[4] -= m1 * s;
            r2[4] -= m2 * s;
            r3[4] -= m3 * s;
        }
        s = r0[5];
        if (s != 0.0)
        {
            r1[5] -= m1 * s;
            r2[5] -= m2 * s;
            r3[5] -= m3 * s;
        }
        s = r0[6];
        if (s != 0.0)
        {
            r1[6] -= m1 * s;
            r2[6] -= m2 * s;
            r3[6] -= m3 * s;
        }
        s = r0[7];
        if (s != 0.0)
        {
            r1[7] -= m1 * s;
            r2[7] -= m2 * s;
            r3[7] -= m3 * s;
        }

        if (fabs(r3[1]) > fabs(r2[1])) SWAP_ROWS(r3, r2);
        if (fabs(r2[1]) > fabs(r1[1])) SWAP_ROWS(r2, r1);
        if (0.0 == r1[1]) return;

        m2 = r2[1] / r1[1];
        m3 = r3[1] / r1[1];
        r2[2] -= m2 * r1[2];
        r3[2] -= m3 * r1[2];
        r2[3] -= m2 * r1[3];
        r3[3] -= m3 * r1[3];
        s = r1[4];
        if (0.0 != s)
        {
            r2[4] -= m2 * s;
            r3[4] -= m3 * s;
        }
        s = r1[5];
        if (0.0 != s)
        {
            r2[5] -= m2 * s;
            r3[5] -= m3 * s;
        }
        s = r1[6];
        if (0.0 != s)
        {
            r2[6] -= m2 * s;
            r3[6] -= m3 * s;
        }
        s = r1[7];
        if (0.0 != s)
        {
            r2[7] -= m2 * s;
            r3[7] -= m3 * s;
        }

        if (fabs(r3[2]) > fabs(r2[2])) SWAP_ROWS(r3, r2);
        if (0.0 == r2[2]) return;

        m3 = r3[2] / r2[2];
        r3[3] -= m3 * r2[3];
        r3[4] -= m3 * r2[4];
        r3[5] -= m3 * r2[5];
        r3[6] -= m3 * r2[6];
        r3[7] -= m3 * r2[7];

        if (0.0 == r3[3]) return;

        s = 1.0 / r3[3];
        r3[4] *= s;
        r3[5] *= s;
        r3[6] *= s;
        r3[7] *= s;

        m2 = r2[3];
        s = 1.0 / r2[2];
        r2[4] = s * (r2[4] - r3[4] * m2);
        r2[5] = s * (r2[5] - r3[5] * m2);
        r2[6] = s * (r2[6] - r3[6] * m2);
        r2[7] = s * (r2[7] - r3[7] * m2);
        m1 = r1[3];
        r1[4] -= r3[4] * m1;
        r1[5] -= r3[5] * m1;
        r1[6] -= r3[6] * m1;
        r1[7] -= r3[7] * m1;
        m0 = r0[3];
        r0[4] -= r3[4] * m0;
        r0[5] -= r3[5] * m0;
        r0[6] -= r3[6] * m0;
        r0[7] -= r3[7] * m0;

        m1 = r1[2];
        s = 1.0 / r1[1];
        r1[4] = s * (r1[4] - r2[4] * m1);
        r1[5] = s * (r1[5] - r2[5] * m1);
        r1[6] = s * (r1[6] - r2[6] * m1);
        r1[7] = s * (r1[7] - r2[7] * m1);
        m0 = r0[2];
        r0[4] -= r2[4] * m0;
        r0[5] -= r2[5] * m0;
        r0[6] -= r2[6] * m0;
        r0[7] -= r2[7] * m0;

        m0 = r0[1];
        s = 1.0 / r0[0];
        r0[4] = s * (r0[4] - r1[4] * m0);
        r0[5] = s * (r0[5] - r1[5] * m0);
        r0[6] = s * (r0[6] - r1[6] * m0);
        r0[7] = s * (r0[7] - r1[7] * m0);

        inMatrix(0, 0) = r0[4];
        inMatrix(0, 1) = r0[5];
        inMatrix(0, 2) = r0[6];
        inMatrix(0, 3) = r0[7];
        inMatrix(1, 0) = r1[4];
        inMatrix(1, 1) = r1[5];
        inMatrix(1, 2) = r1[6];
        inMatrix(1, 3) = r1[7];
        inMatrix(2, 0) = r2[4];
        inMatrix(2, 1) = r2[5];
        inMatrix(2, 2) = r2[6];
        inMatrix(2, 3) = r2[7];
        inMatrix(3, 0) = r3[4];
        inMatrix(3, 1) = r3[5];
        inMatrix(3, 2) = r3[6];
        inMatrix(3, 3) = r3[7];
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& inStream,
        const Matrix4x4<T>& inMatrix)
    {
        inStream << std::setprecision(2);
        for (size_t i = 0; i < 4; ++i)
        {
            if (i) inStream << '\n';
            for (size_t j = 0; j < 4; ++j)
            {
                inStream << std::setw(6) << inMatrix(i, j);
            }
        }

        return inStream;
    }
}

#endif

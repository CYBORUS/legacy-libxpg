#ifndef XPGH_SURFACE
#define XPGH_SURFACE

#include "DataTypes.hpp"

#include <string>

namespace XPG
{
    class Surface
    {
        public:
            Surface();
            Surface(const Surface& inSurface);
            ~Surface();

            inline size_t width() const { return mWidth; }
            inline size_t height() const { return mHeight; }
            inline size_t bitsPerPixel() const { return mBitsPerPixel; }
            inline size_t bytesPerPixel() const { return mBytesPerPixel; }

            void clear();
            void loadImage(const std::string& inFile);
            //void setPixel(size_t inX, size_t inY, vec4f inColor);
            //void loadIntoTexture(GLuint inTexture) const;
            //GLuint loadIntoNewTexture() const;

        private:
            void copy(const Surface& inSurface);

            size_t mWidth;
            size_t mHeight;
            size_t mBitsPerPixel;
            size_t mBytesPerPixel;
            void* mData;
    };
}

#endif


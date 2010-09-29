#include "../Surface.hpp"

#include <cstdlib>

namespace XPG
{
    Surface::Surface() : mWidth(0), mHeight(0), mBitsPerPixel(0),
        mBytesPerPixel(0), mData(NULL)
    {
    }

    Surface::Surface(const Surface& inSurface) : mWidth(0), mHeight(0),
        mBitsPerPixel(0), mBytesPerPixel(0), mData(NULL)
    {
        copy(inSurface);
    }

    Surface::~Surface()
    {
        if (mData) free(mData);
    }

    void Surface::clear()
    {
        mWidth = 0;
        mHeight = 0;
        mBitsPerPixel = 0;
        mBytesPerPixel = 0;
        if (mData) free(mData);
    }

    void Surface::loadImage(const std::string& inFile)
    {
        /// to be implemented
    }

    void Surface::copy(const Surface& inSurface)
    {
        if (inSurface.mData)
        {
            mWidth = inSurface.mWidth;
            mHeight = inSurface.mHeight;
            mBitsPerPixel = inSurface.mBitsPerPixel;
            mBytesPerPixel = inSurface.mBytesPerPixel;
            mData = malloc(mWidth * mHeight * mBytesPerPixel);
            memcpy(mData, inSurface.mData, mWidth * mHeight
                * mBytesPerPixel);
        }
    }
}

#ifndef XPGH_DISPLAY
#define XPGH_DISPLAY

#include "DataTypes.hpp"
#include "Module.hpp"

namespace XPG
{
    class Context
    {
        public:
            Context();
            ~Context();

            inline int32u width() { return mWidth; }
            inline int32u height() { return mHeight; }
            inline int32u depth() { return mDepth; }

            void create(int32u inWidth, int32u inHeight, int32u inDepth = 0,
                int32u inFlags = 0);
            void destroy();
            void runModule(Module* inModule);
            void setWindowTitle(const char* inTitle);
            void setIconTitle(const char* inTitle);

        private:
            int32u mWidth;
            int32u mHeight;
            int32u mDepth;

            int32u mMouseX;
            int32u mMouseY;

            struct PrivateData;
            PrivateData* mData;
    };
}

#endif

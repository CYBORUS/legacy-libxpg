#ifndef XPGH_DISPLAY
#define XPGH_DISPLAY

#include "Event.hpp"
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
            void swapBuffers();
            void dispatchEvents();
            void runModule(Module* inModule);
            void setWindowTitle(const char* inTitle);
            void setIconTitle(const char* inTitle);

            void setWindowListener(WindowEventListener* inListener = NULL);
            void setMouseListener(MouseEventListener* inListener = NULL);
            void setKeyboardListener(KeyboardEventListener* inListener = NULL);

        private:
            int32u mWidth;
            int32u mHeight;
            int32u mDepth;

            WindowEventListener* mWEL;
            MouseEventListener* mMEL;
            KeyboardEventListener* mKEL;

            WindowEventListener mDefaultWindow;
            MouseEventListener mDefaultMouse;
            KeyboardEventListener mDefaultKeyboard;

            struct PrivateData;
            PrivateData* mData;
    };
}

#endif

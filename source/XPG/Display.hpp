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

            struct Parameters
            {
                Parameters();

                int32u width;
                int32u height;
                int32u depth;
                bool fullScreen;
                bool legacyContext;
                int16u versionMajor;
                int16u versionMinor;
                WindowEventListener* WEL;
                MouseEventListener* MEL;
                KeyboardEventListener* KEL;
            };

            inline const Parameters& details() { return mDetails; }

            void create(const Parameters& inDetails = Parameters());
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
            Parameters mDetails;

            WindowEventListener mDefaultWindow;
            MouseEventListener mDefaultMouse;
            KeyboardEventListener mDefaultKeyboard;

            struct PrivateData;
            PrivateData* mData;
    };
}

#endif

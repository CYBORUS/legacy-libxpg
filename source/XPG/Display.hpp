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

            struct Version
            {
                Version();

                int16u vMajor;
                int16u vMinor;
            };

            struct Parameters
            {
                Parameters();

                int32u width;
                int32u height;
                int32u depth;
                bool fullScreen;
                bool legacyContext;
                Version context;
                Version shader;
                WindowEventListener* WEL;
                MouseEventListener* MEL;
                KeyboardEventListener* KEL;
            };

            const Parameters& details;

            void create(const Parameters& inDetails = Parameters());
            void destroy();
            void swapBuffers();
            void dispatchEvents();
            bool getEvent(Event& inEvent);
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

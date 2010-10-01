#include "../Display.hpp"

namespace XPG
{
    void Context::setWindowListener(WindowEventListener* inListener)
    {
        mWEL = inListener ? inListener : &mDefaultWindow;
    }

    void Context::setMouseListener(MouseEventListener* inListener)
    {
        mMEL = inListener ? inListener : &mDefaultMouse;
    }

    void Context::setKeyboardListener(KeyboardEventListener* inListener)
    {
        mKEL = inListener ? inListener : &mDefaultKeyboard;
    }
}

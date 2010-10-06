#include "../Display.hpp"

namespace XPG
{
    Context::Parameters::Parameters() : width(640), height(480), depth(0),
        fullScreen(false), legacyContext(false), versionMajor(0),
        versionMinor(0), WEL(NULL), MEL(NULL), KEL(NULL)
    {

    }

    void Context::setWindowListener(WindowEventListener* inListener)
    {
        mDetails.WEL = inListener ? inListener : &mDefaultWindow;
    }

    void Context::setMouseListener(MouseEventListener* inListener)
    {
        mDetails.MEL = inListener ? inListener : &mDefaultMouse;
    }

    void Context::setKeyboardListener(KeyboardEventListener* inListener)
    {
        mDetails.KEL = inListener ? inListener : &mDefaultKeyboard;
    }
}

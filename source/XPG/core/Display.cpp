#include "../Display.hpp"

namespace XPG
{
    Context::Parameters::Parameters() : width(800), height(600), depth(0),
        fullScreen(false), WEL(NULL), MEL(NULL), KEL(NULL)
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

#include "../Event.hpp"

namespace XPG
{
    void WindowEventListener::onFocus() {}
    void WindowEventListener::onBlur() {}
    void WindowEventListener::onMinimize() {}
    void WindowEventListener::onMaximize() {}
    void WindowEventListener::onRestore() {}
    void WindowEventListener::onResize(int32u inWidth, int32u inHeight) {}
    void WindowEventListener::onExit() {}

    void MouseEventListener::onMove(int32u inX, int32u inY) {}
    void MouseEventListener::onLeaveWindow() {}
    void MouseEventListener::onEnterWindow() {}
    void MouseEventListener::onLeftButtonDown() {}
    void MouseEventListener::onLeftButtonUp() {}
    void MouseEventListener::onRightButtonDown() {}
    void MouseEventListener::onRightButtonUp() {}
    void MouseEventListener::onMiddleButtonDown() {}
    void MouseEventListener::onMiddleButtonUp() {}
    void MouseEventListener::onOtherButtonDown(int8u inButton) {}
    void MouseEventListener::onOtherButtonUp(int8u inButton) {}
    void MouseEventListener::onWheelDown() {}
    void MouseEventListener::onWheelUp() {}

    void KeyboardEventListener::onKeyDown() {}
    void KeyboardEventListener::onKeyRepeat() {}
    void KeyboardEventListener::onKeyUp() {}

    void JoystickEventListener::onAxis() {}
    void JoystickEventListener::onButtonDown() {}
    void JoystickEventListener::onButtonUp() {}
    void JoystickEventListener::onHat() {}
    void JoystickEventListener::onBall() {}
}

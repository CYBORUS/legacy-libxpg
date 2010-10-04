#ifndef XPGH_EVENT
#define XPGH_EVENT

#include "DataTypes.hpp"

namespace XPG
{
    class WindowEventListener
    {
        public:
            virtual void onFocus();
            virtual void onBlur();
            virtual void onMinimize();
            virtual void onMaximize();
            virtual void onRestore();
            virtual void onResize(int32u inWidth, int32u inHeight);
            virtual void onExit();
    };

    class MouseEventListener
    {
        public:
            virtual void onMove(int32u inX, int32u inY);
            virtual void onLeaveWindow();
            virtual void onEnterWindow();
            virtual void onLeftButtonDown() ;
            virtual void onLeftButtonUp();
            virtual void onRightButtonDown();
            virtual void onRightButtonUp();
            virtual void onMiddleButtonDown();
            virtual void onMiddleButtonUp();
            virtual void onOtherButtonDown(int8u inButton);
            virtual void onOtherButtonUp(int8u inButton);
            virtual void onWheelDown();
            virtual void onWheelUp();
    };

    class JoystickEventListener
    {
        public:
            virtual void onAxis();
            virtual void onButtonDown();
            virtual void onButtonUp();
            virtual void onHat();
            virtual void onBall();
    };

    class KeyboardEventListener
    {
        public:
            virtual void onKeyDown();
            virtual void onKeyRepeat();
            virtual void onKeyUp();
    };

    namespace Key
    {
        enum Code
        {
            /// letter keys
            A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W,
            X, Y, Z,

            /// function keys
            F1, F2, F3, F4, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16
        };
    }
}

#endif

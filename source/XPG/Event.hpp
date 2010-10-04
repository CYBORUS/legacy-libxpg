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

    namespace Key
    {
        enum Code
        {
            UNKNOWN,

            /// letter keys
            A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W,
            X, Y, Z,

            /// function keys
            F1, F2, F3, F4, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16,

            /// top row numbers/symbols
            BACK_QUOTE, TR1, TR2, TR3, TR4, TR5, TR6, TR7, TR8, TR9, TR0,
            MINUS, EQUALS, BACKSPACE,

            /// symbols surrounding letter keys
            TAB, LEFT_BRACKET, RIGHT_BRACKET, BACKSLASH, CAPSLOCK, SEMICOLON,
            QUOTE, ENTER, COMMA, PERIOD, SLASH,

            /// mod keys
            LEFT_SHIFT, RIGHT_SHIFT, LEFT_CONTROL, RIGHT_CONTROL, LEFT_ALT,
            RIGHT_ALT, LEFT_SUPER, RIGHT_SUPER,

            /// control keys
            INSERT, HOME, PAGE_UP, PAGE_DOWN, END, DEL, UP, DOWN, LEFT,
            RIGHT,

            /// keypad
            KP0, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9, KP_PERIOD,
            KP_ENTER, KP_PLUS, KP_MINUS, KP_ASTERISK, KP_SLASH, NUMLOCK
        };
    }

    class KeyboardEventListener
    {
        public:
            virtual void onKeyDown(Key::Code inKey);
            virtual void onKeyRepeat(Key::Code inKey);
            virtual void onKeyUp(Key::Code inKey);
    };
}

#endif

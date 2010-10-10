#include "../Display.hpp"
#include "../Timer.hpp"

#include <GL/glew.h>
#include <GL/glxew.h>
#include <X11/Xatom.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
using namespace std;

namespace XPG
{
    Key::Code getKeyCode(int inIndex);

    void SetGLXFunctionPointers()
    {
        static bool done = false;
        if (done) return;
        glGenVertexArraysAPPLE = (void(*)(GLsizei, const GLuint*))
            glXGetProcAddressARB((GLubyte*)"glGenVertexArrays");
        glBindVertexArrayAPPLE = (void(*)(const GLuint))
            glXGetProcAddressARB((GLubyte*)"glBindVertexArray");
        glDeleteVertexArraysAPPLE = (void(*)(GLsizei, const GLuint*))
            glXGetProcAddressARB((GLubyte*)"glGenVertexArrays");

        glXCreateContextAttribsARB = (GLXContext(*)(Display* dpy,
            GLXFBConfig config, GLXContext share_context, Bool direct,
            const int *attrib_list))glXGetProcAddressARB
            ((GLubyte*)"glXCreateContextAttribsARB");
        glXChooseFBConfig = (GLXFBConfig*(*)(Display *dpy, int screen,
            const int *attrib_list, int *nelements))
            glXGetProcAddressARB((GLubyte*)"glXChooseFBConfig");
        glXGetVisualFromFBConfig = (XVisualInfo*(*)(Display *dpy,
            GLXFBConfig config))glXGetProcAddressARB
            ((GLubyte*)"glXGetVisualFromFBConfig");
        done = true;
    }

    struct Context::PrivateData
    {
        GLXContext context;
        Display* display;
        Window window;
        long eventMask;
        Atom wmDeleteMessage;
        bool active;
    };

    Context::Context()
    {
        mData = new PrivateData;
        mData->active = false;
    }

    Context::~Context()
    {
        destroy();
        delete mData;
    }

    void Context::create(const Parameters& inDetails)
    {
        if (mData->active) return;
        mData->active = true;

        mDetails = inDetails;
        setWindowListener(mDetails.WEL);
        setMouseListener(mDetails.MEL);
        setKeyboardListener(mDetails.KEL);

        XSetWindowAttributes winAttribs;
        GLint winmask;
        GLint MajorGLX = 0;
        GLint MinorGLX = 0;
        XVisualInfo* visualInfo;
        GLXFBConfig* fbConfigs;
        int numConfigs = 0;
        static int fbAttribs[] =
            {
                GLX_RENDER_TYPE,   GLX_RGBA_BIT,
                GLX_X_RENDERABLE,  True,
                GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                GLX_DOUBLEBUFFER,  True,
                GLX_RED_SIZE,      8,
                GLX_BLUE_SIZE,     8,
                GLX_GREEN_SIZE,    8,
                0
            };

        SetGLXFunctionPointers();

        mData->display = XOpenDisplay(NULL);

        glXQueryVersion(mData->display, &MajorGLX, &MinorGLX);
        cout << "supported GLX version: " << MajorGLX << '.' << MinorGLX
            << endl;

        if(MajorGLX == 1 && MinorGLX < 2)
        {
            cerr << "ERROR -- GLX 1.2 or greater is required\n";
            XCloseDisplay(mData->display);

            mData->active = false;
            return;
        }

        fbConfigs = glXChooseFBConfig(mData->display,
            DefaultScreen(mData->display), fbAttribs, &numConfigs);
        visualInfo = glXGetVisualFromFBConfig(mData->display, fbConfigs[0]);

        /// X window creation
        mData->eventMask = ExposureMask | VisibilityChangeMask |
            KeyPressMask | PointerMotionMask | StructureNotifyMask
            | ButtonPressMask | ButtonReleaseMask | FocusChangeMask
            | EnterWindowMask | LeaveWindowMask | KeyReleaseMask;
        winAttribs.event_mask = mData->eventMask;

        winAttribs.border_pixel = 0;
        winAttribs.bit_gravity = StaticGravity;
        winAttribs.colormap = XCreateColormap(mData->display,
            RootWindow(mData->display, visualInfo->screen), visualInfo->visual,
            AllocNone);
        winmask = CWBorderPixel | CWBitGravity | CWEventMask| CWColormap;

        int d = mDetails.depth ? mDetails.depth : visualInfo->depth;
        mData->window = XCreateWindow(mData->display,
            DefaultRootWindow(mData->display), 20, 20, mDetails.width,
            mDetails.height, 0, d, InputOutput, visualInfo->visual, winmask,
            &winAttribs);

        // Allow XPG to handle the closing of the window.
        mData->wmDeleteMessage = XInternAtom(mData->display, "WM_DELETE_WINDOW",
            False);
        XSetWMProtocols(mData->display, mData->window,
            &mData->wmDeleteMessage, 1);

        XMapWindow(mData->display, mData->window);

        if (!mDetails.versionMajor)
        {
            mDetails.versionMajor = 3;
            mDetails.versionMinor = 3;
        }

        /// GL context creation
        GLint attribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, mDetails.versionMajor,
            GLX_CONTEXT_MINOR_VERSION_ARB, mDetails.versionMinor,
            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        // Must first check to see if the extension to create an OpenGL 3.x
        // context is even available
        if (glXCreateContextAttribsARB && !mDetails.legacyContext)
        {
            // Good to go. Create the 3.x context.
            mData->context = glXCreateContextAttribsARB(mData->display,
                fbConfigs[0], 0, True, attribs);
        }
        else
        {
            // No good. Create a legacy context.
            mData->context = glXCreateContext(mData->display, visualInfo, NULL,
                True);
            mDetails.legacyContext = true;

            // temporary hack; needs to find actual context version
            mDetails.versionMajor = 2;
            mDetails.versionMinor = 1;
        }

        glXMakeCurrent(mData->display, mData->window, mData->context);

        GLenum e = glewInit();
        if (e != GLEW_OK)
            cerr << "ERROR (GLEW) -- " << glewGetErrorString(e);

        const GLubyte *s = glGetString(GL_VERSION);
        cout << "GL version: " << s << endl;

        if (!mDetails.legacyContext)
        {
            GLint glVersion[2] = {0, 0};
            glGetIntegerv(GL_MAJOR_VERSION, glVersion);
            glGetIntegerv(GL_MINOR_VERSION, glVersion + 1);
            cout << "found version ints: " << glVersion[0] << '.'
                << glVersion[1] << endl;

            mDetails.versionMajor = glVersion[0];
            mDetails.versionMinor = glVersion[1];
        }

        glViewport(0, 0, mDetails.width, mDetails.height);
    }

    void Context::destroy()
    {
        if (mData->active)
        {
            glXMakeCurrent(mData->display, None, NULL);
            glXDestroyContext(mData->display, mData->context);
            XDestroyWindow(mData->display, mData->window);
            XCloseDisplay(mData->display);
            mDetails.width = 0;
            mDetails.height = 0;
            mDetails.depth = 0;
            mData->active = false;
        }
    }

    void Context::swapBuffers()
    {
        glXSwapBuffers(mData->display, mData->window);
    }

    void Context::dispatchEvents()
    {
        while (XEventsQueued(mData->display, QueuedAfterReading))
        {
            XEvent event;
            XNextEvent(mData->display, &event);

            switch(event.type)
            {
                case ButtonPress:
                {
                    switch (event.xbutton.button)
                    {
                        case Button1: mDetails.MEL->onLeftButtonDown(); break;
                        case Button2: mDetails.MEL->onMiddleButtonDown(); break;
                        case Button3: mDetails.MEL->onRightButtonDown(); break;
                        case Button4: mDetails.MEL->onWheelUp(); break;
                        case Button5: mDetails.MEL->onWheelDown(); break;
                        default:
                            mDetails.MEL->onOtherButtonDown
                                (event.xbutton.button);
                    }

                    break;
                }

                case ButtonRelease:
                {
                    switch (event.xbutton.button)
                    {
                        case Button1: mDetails.MEL->onLeftButtonUp(); break;
                        case Button2: mDetails.MEL->onMiddleButtonUp(); break;
                        case Button3: mDetails.MEL->onRightButtonUp(); break;

                        case Button4:
                        case Button5:
                            // Do not report the wheel as a button release
                            break;

                        default:
                            mDetails.MEL->onOtherButtonDown
                                (event.xbutton.button);
                    }

                    break;
                }

                case FocusIn:
                {
                    //cout << "FocusIn" << endl;
                    mDetails.WEL->onFocus();
                    break;
                }

                case FocusOut:
                {
                    //cout << "FocusOut" << endl;
                    mDetails.WEL->onBlur();
                    break;
                }

                case EnterNotify:
                {
                    //cout << "mouse in" << endl;
                    mDetails.MEL->onEnterWindow();
                    break;
                }

                case LeaveNotify:
                {
                    //cout << "mouse out" << endl;
                    mDetails.MEL->onLeaveWindow();
                    break;
                }

                case UnmapNotify:
                {
                    break;
                }

                case MapNotify:
                case ConfigureNotify:
                {
                    XWindowAttributes winData;
                    XGetWindowAttributes(mData->display, mData->window,
                        &winData);
                    mDetails.height = winData.height;
                    mDetails.width = winData.width;
                    glViewport(0, 0, mDetails.width, mDetails.height);
                    mDetails.WEL->onResize(mDetails.width, mDetails.height);
                    break;
                }

                case ClientMessage:
                {
                    //cout << "message" << endl;
                    if (event.xclient.format == 32 &&
                        event.xclient.data.l[0] == mData->wmDeleteMessage)
                    {
                        mDetails.WEL->onExit();
                    }
                    break;
                }

                case MotionNotify:
                {
                    mDetails.MEL->onMove(event.xmotion.x, event.xmotion.y);
                    break;
                }

                case KeyPress:
                {
                    //cout << "key press -- " << event.xkey.keycode << endl;
                    mDetails.KEL->onKeyDown(getKeyCode(event.xkey.keycode));
                    //if (event.xkey.keycode == 9) mDetails.WEL->onExit();
                    break;
                }

                case KeyRelease:
                {
                    bool isRepeat = false;

                    // X11 sends both a KeyPress and a KeyRelease when repeating
                    // a key. The code below distinguishes between a true key
                    // release and a key repeat. If it is a key repeat, it
                    // properly disposes of the subsequent KeyPress message.
                    if (XEventsQueued(mData->display, QueuedAfterReading))
                    {
                        XEvent e;
                        XPeekEvent(mData->display, &e);
                        if (e.type == KeyPress
                            && e.xkey.time == event.xkey.time
                            && e.xkey.keycode == event.xkey.keycode)
                        {
                            //cout << "repeat key -- " << event.xkey.keycode
                            //    << endl;
                            mDetails.KEL->onKeyRepeat(
                                getKeyCode(event.xkey.keycode));
                            XNextEvent(mData->display, &e);
                            isRepeat = true;
                        }
                    }

                    if (!isRepeat)
                    {
                        //cout << "key release -- " << event.xkey.keycode << endl;
                        mDetails.KEL->onKeyUp(getKeyCode(event.xkey.keycode));
                    }

                    break;
                }

                case DestroyNotify:
                {
                    cout << "destroy" << endl;
                    break;
                }

                default:
                {
                    //cout << "no event?" << endl;
                }
            }
        }
    }

    void Context::runModule(Module* inModule)
    {
        if (!mData->active || !inModule) return;

        mDetails.WEL->onResize(mDetails.width, mDetails.height);
        inModule->startRunning();

        while (inModule->isRunning())
        {
            dispatchEvents();
            inModule->onDisplay();
            swapBuffers();
            Idle(1);
        }
    }

    void Context::setWindowTitle(const char* inTitle)
    {
        if (!mData->active || !inTitle) return;

        XTextProperty titleProperty;
        Status status;

        char* t = const_cast<char*>(inTitle);

        status = XStringListToTextProperty(&t, 1, &titleProperty);
        if (status)
        {
            XSetTextProperty(mData->display, mData->window, &titleProperty,
                XA_WM_NAME);
            XFree(titleProperty.value);
        }
    }

    void Context::setIconTitle(const char* inTitle)
    {
        if (!mData->active || !inTitle) return;

        XTextProperty titleProperty;
        Status status;

        char* t = const_cast<char*>(inTitle);

        status = XStringListToTextProperty(&t, 1, &titleProperty);
        if (status)
        {
            XSetTextProperty(mData->display, mData->window, &titleProperty,
                XA_WM_ICON_NAME);
            XFree(titleProperty.value);
        }
    }

    Key::Code getKeyCode(int inIndex)
    {
        switch (inIndex)
        {
            case 38: return Key::A;
            case 56: return Key::B;
            case 54: return Key::C;
            case 40: return Key::D;
            case 26: return Key::E;
            case 41: return Key::F;
            case 42: return Key::G;
            case 43: return Key::H;
            case 31: return Key::I;
            case 44: return Key::J;
            case 45: return Key::K;
            case 46: return Key::L;
            case 58: return Key::M;
            case 57: return Key::N;
            case 32: return Key::O;
            case 33: return Key::P;
            case 24: return Key::Q;
            case 27: return Key::R;
            case 39: return Key::S;
            case 28: return Key::T;
            case 30: return Key::U;
            case 55: return Key::V;
            case 25: return Key::W;
            case 53: return Key::X;
            case 29: return Key::Y;
            case 52: return Key::Z;

            case 19: return Key::TR0;
            case 10: return Key::TR1;
            case 11: return Key::TR2;
            case 12: return Key::TR3;
            case 13: return Key::TR4;
            case 14: return Key::TR5;
            case 15: return Key::TR6;
            case 16: return Key::TR7;
            case 17: return Key::TR8;
            case 18: return Key::TR9;

            case 67: return Key::F1;
            case 68: return Key::F2;
            case 69: return Key::F3;
            case 70: return Key::F4;
            case 71: return Key::F5;
            case 72: return Key::F6;
            case 73: return Key::F7;
            case 74: return Key::F8;
            case 75: return Key::F9;
            case 76: return Key::F10;
            case 95: return Key::F11;
            case 96: return Key::F12;

            case 9: return Key::ESCAPE;
            case 49: return Key::BACK_QUOTE;
            case 20: return Key::MINUS;
            case 21: return Key::EQUALS;
            case 22: return Key::BACKSPACE;
            case 23: return Key::TAB;
            case 34: return Key::LEFT_BRACKET;
            case 35: return Key::RIGHT_BRACKET;
            case 51: return Key::BACKSLASH;
            case 66: return Key::CAPSLOCK;
            case 47: return Key::SEMICOLON;
            case 48: return Key::QUOTE;
            case 36: return Key::ENTER;
            case 59: return Key::COMMA;
            case 60: return Key::PERIOD;
            case 61: return Key::SLASH;
            case 65: return Key::SPACE;

            case 50: return Key::LEFT_SHIFT;
            case 62: return Key::RIGHT_SHIFT;
            case 37: return Key::LEFT_CONTROL;
            case 105: return Key::RIGHT_CONTROL;
            case 64: return Key::LEFT_ALT;
            case 108: return Key::RIGHT_ALT;
            case 133: return Key::LEFT_SUPER;
            case 134: return Key::RIGHT_SUPER;

            case 78: return Key::SCROLL_LOCK;
            case 127: return Key::PAUSE;
            // need key code for Print Screen, if possible

            case 118: return Key::INSERT;
            case 110: return Key::HOME;
            case 112: return Key::PAGE_UP;
            case 117: return Key::PAGE_DOWN;
            case 115: return Key::END;
            case 119: return Key::DEL;

            case 111: return Key::UP;
            case 116: return Key::DOWN;
            case 113: return Key::LEFT;
            case 114: return Key::RIGHT;

            case 90: return Key::KP0;
            case 87: return Key::KP1;
            case 88: return Key::KP2;
            case 89: return Key::KP3;
            case 83: return Key::KP4;
            case 84: return Key::KP5;
            case 85: return Key::KP6;
            case 79: return Key::KP7;
            case 80: return Key::KP8;
            case 81: return Key::KP9;

            case 77: return Key::NUMLOCK;
            case 106: return Key::KP_SLASH;
            case 63: return Key::KP_ASTERISK;
            case 82: return Key::KP_MINUS;
            case 86: return Key::KP_PLUS;
            case 104: return Key::KP_ENTER;
            case 91: return Key::KP_PERIOD;

            default:
                return Key::UNKNOWN;
        }
    }
}

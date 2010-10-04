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

        /// GL context creation
        GLint attribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 1,
            //GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        // Must first check to see if the extension to create an OpenGL 3.x
        // context is even available
        if (glXCreateContextAttribsARB)
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
        }

        glXMakeCurrent(mData->display, mData->window, mData->context);

        GLenum e = glewInit();
        if (e != GLEW_OK)
            cerr << "ERROR (GLEW) -- " << glewGetErrorString(e);

        const GLubyte *s = glGetString(GL_VERSION);
        cout << "GL version: " << s << endl;

        int glVersion[2] = {-1, -1};
        glGetIntegerv(GL_MAJOR_VERSION, glVersion);
        glGetIntegerv(GL_MINOR_VERSION, glVersion + 1);
        cout << "found version ints: " << glVersion[0] << '.' << glVersion[1]
            << endl;

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
        XEvent event;
        XWindowAttributes winData;

        while (XCheckWindowEvent(mData->display, mData->window,
            mData->eventMask, &event))
        {
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
                    cout << "message" << endl;
//                    if (event.xclient.format == 32 &&
//                        event.xclient.data.l[0] == wmDeleteMessage)
//                    {
//                        cout << "quit" << endl;
//                    }
                    break;
                }

                case MotionNotify:
                {
                    mDetails.MEL->onMove(event.xmotion.x, event.xmotion.y);
                    break;
                }

                case KeyPress:
                {
                    cout << "key press -- " << event.xkey.keycode << endl;
                    if (event.xkey.keycode == 9) mDetails.WEL->onExit();
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
                            cout << "repeat key -- " << event.xkey.keycode
                                << endl;
                            XNextEvent(mData->display, &e);
                            isRepeat = true;
                        }
                    }

                    if (!isRepeat)
                    {
                        cout << "key release -- " << event.xkey.keycode << endl;
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
                    cout << "no event?" << endl;
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
}

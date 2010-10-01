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
    };

    Context::Context() : mWidth(0), mHeight(0), mDepth(0)
    {
        mData = new PrivateData;
        mMEL = &mDefaultMouse;
        mKEL = &mDefaultKeyboard;
        mWEL = &mDefaultWindow;
    }

    Context::~Context()
    {
        destroy();
        delete mData;
    }

    void Context::create(int32u inWidth, int32u inHeight, int32u inDepth,
        int32u inFlags)
    {
        // mWidth will be set to a non-zero value if this context is already in
        // use. mHeight and mDepth will be set as well, but mWidth is checked
        // everywhere simply to be consistent.
        if (mWidth) return;

        mWidth = inWidth;
        mHeight = inHeight;

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
            mWidth = 0;
            mHeight = 0;
            mDepth = 0;
            return;
        }

        fbConfigs = glXChooseFBConfig(mData->display,
            DefaultScreen(mData->display), fbAttribs, &numConfigs);
        visualInfo = glXGetVisualFromFBConfig(mData->display, fbConfigs[0]);

        /// X window creation
        mData->eventMask = ExposureMask | VisibilityChangeMask |
            KeyPressMask | PointerMotionMask | StructureNotifyMask
            | ButtonPressMask | ButtonReleaseMask | FocusChangeMask
            | EnterWindowMask | LeaveWindowMask;
        winAttribs.event_mask = mData->eventMask;

        winAttribs.border_pixel = 0;
        winAttribs.bit_gravity = StaticGravity;
        winAttribs.colormap = XCreateColormap(mData->display,
            RootWindow(mData->display, visualInfo->screen), visualInfo->visual,
            AllocNone);
        winmask = CWBorderPixel | CWBitGravity | CWEventMask| CWColormap;

        int d = mDepth ? mDepth : visualInfo->depth;
        mData->window = XCreateWindow(mData->display,
            DefaultRootWindow(mData->display), 20, 20, mWidth, mHeight, 0,
            d, InputOutput, visualInfo->visual, winmask,
            &winAttribs);

        XMapWindow(mData->display, mData->window);

        /// GL context creation
        GLint attribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 1,
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

        glViewport(0, 0, mWidth, mHeight);
    }

    void Context::destroy()
    {
        if (mWidth)
        {
            glXMakeCurrent(mData->display, None, NULL);
            glXDestroyContext(mData->display, mData->context);
            XDestroyWindow(mData->display, mData->window);
            XCloseDisplay(mData->display);
            mWidth = 0;
            mHeight = 0;
            mDepth = 0;
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
        //XNextEvent(mData->display, &event);

        if (XCheckWindowEvent(mData->display, mData->window,
            mData->eventMask, &event))
        {
            switch(event.type)
            {
                case ButtonPress:
                {
                    switch (event.xbutton.button)
                    {
                        case Button1: mMEL->onLeftButtonDown(); break;
                        case Button2: mMEL->onMiddleButtonDown(); break;
                        case Button3: mMEL->onRightButtonDown(); break;
                        case Button4: mMEL->onWheelUp(); break;
                        case Button5: mMEL->onWheelDown(); break;
                        default:
                            mMEL->onOtherButtonDown(event.xbutton.button);
                    }

                    break;
                }

                case ButtonRelease:
                {
                    switch (event.xbutton.button)
                    {
                        case Button1: mMEL->onLeftButtonUp(); break;
                        case Button2: mMEL->onMiddleButtonUp(); break;
                        case Button3: mMEL->onRightButtonUp(); break;

                        case Button4:
                        case Button5:
                            // Do not report the wheel as a button release
                            break;

                        default:
                            mMEL->onOtherButtonDown
                                (event.xbutton.button);
                    }

                    break;
                }

                case FocusIn:
                {
                    //cout << "FocusIn" << endl;
                    mWEL->onFocus();
                    break;
                }

                case FocusOut:
                {
                    //cout << "FocusOut" << endl;
                    mWEL->onBlur();
                    break;
                }

                case EnterNotify:
                {
                    //cout << "mouse in" << endl;
                    mMEL->onEnterWindow();
                    break;
                }

                case LeaveNotify:
                {
                    //cout << "mouse out" << endl;
                    mMEL->onLeaveWindow();
                    break;
                }

                case UnmapNotify:
                {
                    break;
                }

                case MapNotify:
                {
                }

                case ConfigureNotify:
                {
                    XGetWindowAttributes(mData->display, mData->window,
                        &winData);
                    mHeight = winData.height;
                    mWidth = winData.width;
                    glViewport(0, 0, mWidth, mHeight);
                    mWEL->onResize(mWidth, mHeight);
                    break;
                }

                case MotionNotify:
                {
                    mMEL->onMove(event.xmotion.x, event.xmotion.y);
                    break;
                }

                case KeyPress:
                {
                    cout << "key code -- " << event.xkey.keycode << endl;
                    if (event.xkey.keycode == 9) mWEL->onExit();
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
        if (!mWidth || !inModule) return;

        inModule->onResize(mWidth, mHeight);
        inModule->startRunning();

        int64u f = 0;
        while (inModule->isRunning())
        {
            dispatchEvents();
            inModule->onDisplay();
            swapBuffers();
            //cout << f++ << endl;
            Idle(1);
        }
    }

    void Context::setWindowTitle(const char* inTitle)
    {
        if (!mWidth || !inTitle) return;

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
        if (!mWidth || !inTitle) return;

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

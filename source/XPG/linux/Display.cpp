#include "../Display.hpp"

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
    };

    Context::Context() : mWidth(0), mHeight(0), mDepth(0)
    {
        mData = new PrivateData;
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
        mMouseX = 0;
        mMouseY = 0;

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
        winAttribs.event_mask = ExposureMask | VisibilityChangeMask |
            KeyPressMask | PointerMotionMask | StructureNotifyMask;

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

    void Context::runModule(Module* inModule)
    {
        if (!mWidth || !inModule) return;

        Bool bWinMapped = False;
        inModule->onResize(mWidth, mHeight);
        inModule->startRunning();

        while (inModule->isRunning())
        {
            XEvent event;
            XWindowAttributes winData;
            XNextEvent(mData->display, &event);

            switch(event.type)
            {
                case UnmapNotify:
                {
                    bWinMapped = False;
                    break;
                }

                case MapNotify:
                {
                    bWinMapped = True;
                }

                case ConfigureNotify:
                {
                    XGetWindowAttributes(mData->display, mData->window,
                        &winData);
                    mHeight = winData.height;
                    mWidth = winData.width;
                    glViewport(0, 0, mWidth, mHeight);
                    inModule->onResize(mWidth, mHeight);
                    break;
                }

                case MotionNotify:
                {
                    mMouseX = event.xmotion.x;
                    mMouseY = event.xmotion.y;
                    inModule->onMouseMove(mMouseX, mMouseY);
                    inModule->onDisplay();
                    glXSwapBuffers(mData->display, mData->window);
                    break;
                }

                case KeyPress:
                case DestroyNotify:
                {
                    inModule->stopRunning();
                    break;
                }

                default:
                {}
            }

            if(bWinMapped)
            {
                inModule->onDisplay();
                glXSwapBuffers(mData->display, mData->window);
            }
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

#include "../Display.hpp"

#include <GL/glew.h>
#include <GL/glxew.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace XPG
{
    void EarlyInitGLXfnPointers()
    {
        static bool done = false;
        if (done) return;
        glGenVertexArraysAPPLE = (void(*)(GLsizei, const GLuint*))glXGetProcAddressARB((GLubyte*)"glGenVertexArrays");
        glBindVertexArrayAPPLE = (void(*)(const GLuint))glXGetProcAddressARB((GLubyte*)"glBindVertexArray");
        glDeleteVertexArraysAPPLE = (void(*)(GLsizei, const GLuint*))glXGetProcAddressARB((GLubyte*)"glGenVertexArrays");
        glXCreateContextAttribsARB = (GLXContext(*)(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list))glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
        glXChooseFBConfig = (GLXFBConfig*(*)(Display *dpy, int screen, const int *attrib_list, int *nelements))glXGetProcAddressARB((GLubyte*)"glXChooseFBConfig");
        glXGetVisualFromFBConfig = (XVisualInfo*(*)(Display *dpy, GLXFBConfig config))glXGetProcAddressARB((GLubyte*)"glXGetVisualFromFBConfig");
        done = true;
    }

    struct Context::PrivateData
    {
        GLXContext context;
        Display* display;
        Window window;
    };

    Context::Context() : mData(NULL)
    {
    }

    Context::~Context()
    {
        destroy();
    }

    void Context::create(int32u inWidth, int32u inHeight, int32u inDepth,
        int32u inFlags)
    {
        if (mData) return;

        mData = new PrivateData;

        mWidth = inWidth;
        mHeight = inHeight;
        mMouseX = 0;
        mMouseY = 0;

        XSetWindowAttributes winAttribs;
        GLint winmask;
        GLint nMajorVer = 0;
        GLint nMinorVer = 0;
        XVisualInfo *visualInfo;
        GLXFBConfig *fbConfigs;
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

        EarlyInitGLXfnPointers();

        // Tell X we are going to use the display
        mData->display = XOpenDisplay(NULL);

        // Get Version info
        glXQueryVersion(mData->display, &nMajorVer, &nMinorVer);
        printf("Supported GLX version - %d.%d\n", nMajorVer, nMinorVer);

        if(nMajorVer == 1 && nMinorVer < 2)
        {
            printf("ERROR: GLX 1.2 or greater is necessary\n");
            XCloseDisplay(mData->display);

            delete mData;
            mData = NULL;
            return;
        }

        // Get a new fb config that meets our attrib requirements
        fbConfigs = glXChooseFBConfig(mData->display,
            DefaultScreen(mData->display), fbAttribs, &numConfigs);
        visualInfo = glXGetVisualFromFBConfig(mData->display, fbConfigs[0]);

        // Now create an X window
        winAttribs.event_mask = ExposureMask | VisibilityChangeMask |
            KeyPressMask | PointerMotionMask | StructureNotifyMask;

        winAttribs.border_pixel = 0;
        winAttribs.bit_gravity = StaticGravity;
        winAttribs.colormap = XCreateColormap(mData->display,
            RootWindow(mData->display, visualInfo->screen), visualInfo->visual,
            AllocNone);
        winmask = CWBorderPixel | CWBitGravity | CWEventMask| CWColormap;

        mData->window = XCreateWindow(mData->display,
            DefaultRootWindow(mData->display), 20, 20, mWidth, mHeight, 0,
            visualInfo->depth, InputOutput, visualInfo->visual, winmask,
            &winAttribs);

        XMapWindow(mData->display, mData->window);

        // Also create a new GL context for rendering
        GLint attribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 1,
            0
        };

        mData->context = glXCreateContextAttribsARB(mData->display,
            fbConfigs[0], 0, True, attribs);
        glXMakeCurrent(mData->display, mData->window, mData->context);

        GLenum e = glewInit();
        if (e != GLEW_OK)
        {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(e));
        }

        const GLubyte *s = glGetString(GL_VERSION);
        printf("GL Version = %s\n", s);

        glViewport(0, 0, mWidth, mHeight);
    }

    void Context::destroy()
    {
        if (mData)
        {
            glXMakeCurrent(mData->display, None, NULL);
            glXDestroyContext(mData->display, mData->context);
            XDestroyWindow(mData->display, mData->window);
            XCloseDisplay(mData->display);
            delete mData;
			mData = NULL;
        }
    }

    void Context::runModule(Module* inModule)
    {
        if (!mData || !inModule) return;

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
}

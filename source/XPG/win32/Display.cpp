#include "../Display.hpp"
#include "../OperatingSystems.hpp"

#include <GL/glew.h>
#include <GL/wglew.h>

#include <iostream>
using namespace std;

namespace XPG
{
    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static Module* activeModule;
    static HDC activeHDC;
    static int32u* activeWidth;
    static int32u* activeHeight;

    struct Context::PrivateData
    {
        HWND hWnd;
        HGLRC hrc;
        HDC hdc;
        HINSTANCE hInstance;
        char title[255];
    };

    Context::Context() : mWidth(0), mHeight(0), mDepth(0)
    {
        mData = new PrivateData;
        strcpy(mData->title, "OpenGL 3");
    }

    Context::~Context()
    {
        destroy();
        delete mData;
    }

    void Context::create(int32u inWidth, int32u inHeight, int32u inDepth,
                         int32u inFlags)
    {
        if (mWidth) return;

        mWidth = inWidth;
        mHeight = inHeight;
        mDepth = inDepth;

        mData->hInstance = GetModuleHandle(NULL);
        WNDCLASS windowClass;
        DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = (WNDPROC) WndProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = mData->hInstance;
        windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = NULL;
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = mData->title;

        if (!RegisterClass(&windowClass))
        {
            cerr << "fail on RegisterClass\n";
            return;
        }

        mData->hWnd = CreateWindowEx(dwExStyle, mData->title, mData->title,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, mWidth, mHeight, NULL, NULL,
            mData->hInstance, NULL);

        mData->hdc = GetDC(mData->hWnd); // Get the device context for our window

        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL |
            PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = mDepth; // Give us 32 bits of color information (the higher, the more colors)
        pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)
        pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD

        int nPixelFormat = ChoosePixelFormat(mData->hdc, &pfd); // Check if our PFD is valid and get a pixel format back
        if (nPixelFormat == 0) // If it fails
        {
            cerr << "failed ChoosePixelFormat(hdc, &pfd)" << endl;
            return;
        }

        bool bResult = SetPixelFormat(mData->hdc, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD
        if (!bResult) return;

        HGLRC tempOpenGLContext = wglCreateContext(mData->hdc); // Create an OpenGL 2.1 context for our device context
        wglMakeCurrent(mData->hdc, tempOpenGLContext); // Make the OpenGL 2.1 context current and active

        GLenum e = glewInit(); // Enable GLEW
        if (e != GLEW_OK) // If GLEW fails
        {
            cerr << "failed to init GLEW" << endl;
            return;
        }

        int attributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3
            WGL_CONTEXT_MINOR_VERSION_ARB, 0, // Set the MINOR version of OpenGL to 2
            //WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
            0
        };

        if (wglewIsSupported("WGL_ARB_create_context") == 1) { // If the OpenGL 3.x context creation extension is available
            mData->hrc = wglCreateContextAttribsARB(mData->hdc, NULL, attributes); // Create and OpenGL 3.x context based on the given attributes
            wglMakeCurrent(NULL, NULL); // Remove the temporary context from being active
            wglDeleteContext(tempOpenGLContext); // Delete the temporary OpenGL 2.1 context
            wglMakeCurrent(mData->hdc, mData->hrc); // Make our OpenGL 3.0 context current
            cerr << "replacing context with OGL 3.0" << endl;
        }
        else {
            cerr << "resorting to OGL 2.1 context" << endl;
            mData->hrc = tempOpenGLContext; // If we didn't have support for OpenGL 3.x and up, use the OpenGL 2.1 context
        }

        int glVersion[2] = {-1, -1}; // Set some default values for the version
        glGetIntegerv(GL_MAJOR_VERSION, glVersion); // Get back the OpenGL MAJOR version we are using
        glGetIntegerv(GL_MINOR_VERSION, glVersion + 1); // Get back the OpenGL MAJOR version we are using


        cerr << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << endl; // Output which version of OpenGL we are using

        ShowWindow(mData->hWnd, SW_SHOW);
        UpdateWindow(mData->hWnd);

        glViewport(0, 0, mWidth, mHeight); // Set the viewport size to fill the window
    }

    void Context::destroy()
    {
        if (mWidth)
        {
            wglMakeCurrent(mData->hdc, 0); // Remove the rendering context from our device context
            wglDeleteContext(mData->hrc); // Delete our rendering context
            ReleaseDC(mData->hWnd, mData->hdc); // Release the device context from our window
            mWidth = 0;
            mHeight = 0;
            mDepth = 0;
        }
    }

    void Context::runModule(Module* inModule)
    {
        if (!mWidth || !inModule) return;

        inModule->onResize(mWidth, mHeight);
        inModule->startRunning();
        activeModule = inModule;
        activeHDC = mData->hdc;
        activeWidth = &mWidth;
        activeHeight = &mHeight;

        while (inModule->isRunning())
        {
            MSG msg;
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {   // If we have a message to process, process it
                if (msg.message == WM_QUIT)
                {
                    inModule->stopRunning(); // Set running to false if we have a message to quit
                }
                else
                {
                    if (msg.message == WM_LBUTTONDOWN)
                        setWindowTitle("1337 h4x!");
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            else
            {   // If we don't have a message to process
                inModule->onDisplay();
                SwapBuffers(mData->hdc); // Swap buffers so we can see our rendering
            }
        }
    }

    void Context::setWindowTitle(const char* inTitle)
    {
        if (!inTitle || strlen(inTitle) > 254) return;

        strcpy(mData->title, inTitle);
        if (mWidth)
        {
            SetWindowText(mData->hWnd, mData->title);
            /// TODO: add error checking
        }
    }

    /// /// ///

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
            case WM_MBUTTONDOWN:
            {
                cout << "mouse MButton down" << endl;
                break;
            }

            case WM_MBUTTONUP:
            {
                cout << "mouse MButton up" << endl;
                break;
            }

            case WM_LBUTTONDOWN:
            {
                cout << "mouse LButton down" << endl;
                break;
            }

            case WM_LBUTTONUP:
            {
                //MessageBox(hWnd, "Greetings from TheBuzzSaw", "Welcome", MB_OK);
                cout << "mouse LButton up" << endl;
                break;
            }

            case WM_RBUTTONDOWN:
            {
                cout << "mouse RButton down" << endl;
                break;
            }

            case WM_RBUTTONUP:
            {
                cout << "mouse RButton up" << endl;
                break;
            }

            case WM_MOUSELEAVE:
            {
                cout << "mouse leave" << endl;
                break;
            }

            case WM_MOUSEMOVE:
            {
                // http://msdn.microsoft.com/en-us/library/ms632654%28v=VS.85%29.aspx
                //cout << "mouse move: " << GET_X_LPARAM(lparam) << ' '
                //     << GET_Y_LPARAM(lparam) << endl;
                activeModule->onMouseMove(GET_X_LPARAM(lparam),
                    GET_Y_LPARAM(lparam));
                break;
            }

            case WM_MOUSEWHEEL:
            {
                //http://msdn.microsoft.com/en-us/library/ms645617%28v=VS.85%29.aspx
                cout << "mouse wheel: " << GET_WHEEL_DELTA_WPARAM(wparam) << endl;
                break;
            }

            //case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC dc;
                RECT r;
                GetClientRect(hWnd,&r);
                dc=BeginPaint(hWnd,&ps);
                DrawText(dc,"Hello World",-1,&r,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
                EndPaint(hWnd,&ps);
                break;
            }

            // http://msdn.microsoft.com/en-us/library/ff468861%28v=VS.85%29.aspx
            case WM_SETFOCUS:
            {
                cout << "focus" << endl;
                break;
            }

            case WM_KILLFOCUS:
            {
                cout << "blur" << endl;
                break;
            }

            case WM_KEYDOWN:
            {
                // http://msdn.microsoft.com/en-us/library/ms646280%28VS.85%29.aspx
                // Bit 30 of lparam indicates last key state.
                // We do not want to repeat this event.
                if (lparam & 0x40000000) break;

                unsigned int key = (unsigned int)wparam;
                cout << "key down -- " << key << endl;
                if (key == 27) PostQuitMessage(0);
                break;
            }

            case WM_KEYUP:
            {
                unsigned int key = (unsigned int)wparam;
                cout << "key up -- " << key << endl;
                break;
            }

            case WM_SIZE:
            {
                *activeWidth = LOWORD(lparam); // Set the window width
                *activeHeight = HIWORD(lparam); // Set the window height
                glViewport(0, 0, *activeWidth, *activeHeight);
                activeModule->onResize(*activeWidth, *activeHeight);
                activeModule->onDisplay();
                SwapBuffers(activeHDC); // Swap buffers so we can see our rendering


                // http://msdn.microsoft.com/en-us/library/ms632646%28v=VS.85%29.aspx
                switch (wparam)
                {
                    case SIZE_MAXIMIZED:
                    {
                        cout << "maximize" << endl;
                        break;
                    }

                    case SIZE_MINIMIZED:
                    {
                        cout << "minimize" << endl;
                        break;
                    }

                    case SIZE_RESTORED:
                    {
                        cout << "restore" << endl;
                        break;
                    }

                    default:
                    {}
                }
                break;
            }

            case WM_CREATE:
            {
                break;
            }

            case WM_DESTROY:
                PostQuitMessage(0);
                break;

            default:
                return DefWindowProc(hWnd, msg, wparam, lparam);
        }
        return 0;
    }
}

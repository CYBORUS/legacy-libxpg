#ifndef TESTMODULE_H
#define TESTMODULE_H

#include <XPG/Module.hpp>
#include <XPG/Event.hpp>

class TestModule : public XPG::Module, public XPG::MouseEventListener,
    public XPG::WindowEventListener
{
    public:
        TestModule();
        virtual ~TestModule();

        /// event handlers
        virtual void onExit();
        virtual void onMove(int32u inX, int32u inY);
        virtual void onResize(int32u inWidth, int32u inHeight);
        virtual void onDisplay();

    private:
        void DrawCircle();
        void Draw();

        int32u mWidth;
        int32u mHeight;
        int32u mMouseX;
        int32u mMouseY;
};

#endif

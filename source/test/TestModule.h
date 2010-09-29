#ifndef TESTMODULE_H
#define TESTMODULE_H

#include <XPG/Module.hpp>

class TestModule : public XPG::Module
{
    public:
        TestModule();
        virtual ~TestModule();

        virtual void onMouseMove(int32u inX, int32u inY);
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

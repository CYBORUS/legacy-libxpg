#ifndef FANCYTESTMODULE_H
#define FANCYTESTMODULE_H

#include <XPG/Module.hpp>
#include <XPG/Event.hpp>
#include <XPG/Program.hpp>
#include <XPG/ClusterVBO.hpp>

class FancyTestModule : public XPG::Module, public XPG::MouseEventListener,
    public XPG::KeyboardEventListener, public XPG::WindowEventListener
{
    public:
        FancyTestModule();
        virtual ~FancyTestModule();

        virtual void onDisplay();
        virtual void onKeyDown(XPG::Key::Code inKey);
        virtual void onResize(int32u inWidth, int32u inHeight);

    private:
        XPG::Program mProgram;
        XPG::Shader mVS;
        XPG::Shader mFS;
        XPG::ClusterVBO<2> mVBO;

        GLint mUniMVPM;
        mat4f mProjection;
        mat4f mModelView;
        mat4f mMVP;
};

#endif

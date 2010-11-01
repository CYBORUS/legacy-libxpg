#ifndef FANCYTESTMODULE_H
#define FANCYTESTMODULE_H

#include <XPG/ClusterVBO.hpp>
#include <XPG/IndexVBO.hpp>
#include <XPG/Module.hpp>
#include <XPG/Event.hpp>
#include <XPG/Program.hpp>

class FancyTestModule : public XPG::Module, public XPG::MouseEventListener,
    public XPG::KeyboardEventListener, public XPG::WindowEventListener
{
    public:
        FancyTestModule();
        virtual ~FancyTestModule();

        virtual void onExit();
        virtual void onDisplay();
        virtual void onKeyDown(XPG::Key::Code inKey);
        virtual void onResize(int32u inWidth, int32u inHeight);

    private:
        XPG::Program<2> mProgram;
        XPG::Shader mVS;
        XPG::Shader mFS;
        XPG::ClusterVBO<3> mVBO;
        XPG::IndexVBO mIVBO;

        GLint mUniMVPM;
        GLint mUniMVM;
        GLint mUniNM;
        mat4f mProjection;
        mat4f mModelView;
        mat4f mNormalView;
        float mRotate;
        int64u mNextFrame;
};

#endif

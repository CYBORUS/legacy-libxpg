#ifndef FANCYTESTMODULE_H
#define FANCYTESTMODULE_H

#include <XPG/ClusterVBO.hpp>
#include <XPG/IndexVBO.hpp>
#include <XPG/Module.hpp>
#include <XPG/Program.hpp>

class FancyTestModule : public XPG::Module
{
    public:
        FancyTestModule(int16u inMajorVersion);
        virtual ~FancyTestModule();

        void onExit();
        void onKeyDown(XPG::Key::Code inKey);
        void onResize(int32u inWidth, int32u inHeight);

        virtual void handleEvent(const XPG::Event& inEvent);
        virtual void onDisplay();

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
        bool mLegacy;
};

#endif

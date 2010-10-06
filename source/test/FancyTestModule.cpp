#include "FancyTestModule.h"

#include <iostream>
using namespace std;

FancyTestModule::FancyTestModule()
{
    mVS.load("test.vs", GL_VERTEX_SHADER);
    mFS.load("test.fs", GL_FRAGMENT_SHADER);
    mProgram.attachShader(mVS);
    mProgram.attachShader(mFS);
    mProgram.link();
    mUniMVPM = mProgram.getUniformLocation("MVPM");

}

FancyTestModule::~FancyTestModule()
{
    mProgram.clear();
}

void FancyTestModule::onKeyDown(XPG::Key::Code inKey)
{
    using namespace XPG;

    switch (inKey)
    {
        case Key::ESCAPE:
        {
            stopRunning();
            break;
        }

        default: {}
    }
}

void FancyTestModule::onResize(int32u inWidth, int32u inHeight)
{
    float ratio = static_cast<float>(inWidth) / static_cast<float>(inHeight);
    mProjection.loadIdentity();
    mProjection.perspective(30.0f, ratio, 1.0f, 1000.0f);
}

void FancyTestModule::onDisplay()
{
    mModelView.loadIdentity();

    (mMVP = mProjection).multiply(mModelView);
    glUniformMatrix4fv(mUniMVPM, 1, GL_FALSE, mMVP.array());
}

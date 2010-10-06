#include "FancyTestModule.h"

#define LDEBUG cout << "yarr -- " << __LINE__ << endl

#include <iostream>
using namespace std;

FancyTestModule::FancyTestModule()
{
    mVS.load("test.vs", GL_VERTEX_SHADER);
    mFS.load("test.fs", GL_FRAGMENT_SHADER);
    mProgram.attachShader(mVS);
    mProgram.attachShader(mFS);
    mProgram.bindAttribLocation(0, "in_Position");
    mProgram.bindAttribLocation(1, "in_Color");
    mProgram.link();
    mUniMVPM = mProgram.getUniformLocation("MVPM");

    GLfloat triangle[9] = {
        -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f
        };

    mVBO.loadVAA(0, 3, 3, triangle);

    GLfloat colors[12] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
        };

    mVBO.loadVAA(1, 4, 3, colors);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
    //mProjection.perspective(30.0f, ratio, 1.0f, 1000.0f);
}

void FancyTestModule::onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mModelView.loadIdentity();

    (mMVP = mProjection).multiply(mModelView);
    glUniformMatrix4fv(mUniMVPM, 1, GL_FALSE, mMVP.array());
    mVBO.display(GL_TRIANGLES, 0, 3);
}

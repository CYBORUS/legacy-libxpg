#include "FancyTestModule.h"
#include <cmath>

#define LDEBUG cout << "yarr -- " << __LINE__ << endl

#include <iostream>
using namespace std;

FancyTestModule::FancyTestModule() : mRotate(0.0f)
{
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    mVS.load("test.vs", GL_VERTEX_SHADER);
    mFS.load("test.fs", GL_FRAGMENT_SHADER);
    mProgram.attachShader(mVS);
    mProgram.attachShader(mFS);
    mProgram.bindAttribLocation(0, "in_Position");
    mProgram.bindAttribLocation(1, "in_Color");
    mProgram.bindAttribLocation(2, "in_Normal");
    mProgram.link();
    mUniMVPM = mProgram.getUniformLocation("MVPM");

    GLint u;
    vec4f v(0.0f, 0.0f, 0.0f, 1.0f);
    u = mProgram.getUniformLocation("ambientColor");
    glUniform4fv(u, 4, v.array());

    u = mProgram.getUniformLocation("diffuseColor");
    glUniform4fv(u, 4, v.array());

    u = mProgram.getUniformLocation("specularColor");
    glUniform4fv(u, 4, v.array());

    u = mProgram.getUniformLocation("lightPosition");
    glUniform4fv(u, 3, v.array());

    GLfloat points[24] = {
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f
        };

    mVBO.loadVAA(0, 3, 8, points);

    GLfloat colors[32] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        };

    mVBO.loadVAA(1, 4, 8, colors);

    GLfloat a = 1.0f / sqrt(3.0f);
    GLfloat normals[24] = {
        a, a, a,
        a, -a, a,
        -a, -a, a,
        -a, a, a,
        a, a, -a,
        a, -a, -a,
        -a, -a, -a,
        -a, a, -a,
        };

    mVBO.loadVAA(2, 3, 8, normals);

    GLuint indices[36] = {
            0, 1, 2, 0, 2, 3, // top
            7, 6, 5, 7, 5, 4, // bottom
            1, 0, 4, 1, 4, 5, // right
            3, 2, 6, 3, 6, 7, // left
            2, 1, 5, 2, 5, 6, // front
            0, 3, 7, 0, 7, 4  // back
        };

    mIVBO.loadData(GL_TRIANGLES, 36, indices);

    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
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
    //mProjection.orthographic(2.0f, ratio);
    mProjection.perspective(30.0f, ratio, 1.0f, 1000.0f);
}

void FancyTestModule::onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mModelView.loadIdentity();
    mModelView.translate(0.0f, 0.0f, -10.0f);
    mModelView.rotateX(mRotate);
    mModelView.rotateZ(mRotate);
    mRotate += 0.05f;
    if (mRotate > 180.0f) mRotate -= 360.0f;

    (mMVP = mProjection).multiply(mModelView);
    glUniformMatrix4fv(mUniMVPM, 1, GL_FALSE, mMVP.array());
    mVBO.display(mIVBO);
}

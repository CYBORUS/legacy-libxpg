#include "TestModule.h"

#include <iostream>
using namespace std;

TestModule::TestModule()
{
}

TestModule::~TestModule()
{
}

void TestModule::onExit()
{
    stopRunning();
}

void TestModule::onMove(int32u inX, int32u inY)
{
    mMouseX = inX;
    mMouseY = inY;
}

void TestModule::onResize(int32u inWidth, int32u inHeight)
{
    mWidth = inWidth;
    mHeight = inHeight;

    float aspectRatio = inHeight ? static_cast<float>(inWidth)
        / static_cast<float>(inHeight) : 1.0f;
    float fYTop     = 0.05f;
    float fYBottom  = - fYTop;
    float fXLeft    = fYTop     * aspectRatio;
    float fXRight   = fYBottom  * aspectRatio;

    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Clear matrix stack
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set the frustrum
    glFrustum(fXLeft, fXRight, fYBottom, fYTop, 0.1f, 100.f);
}

void TestModule::onDisplay()
{
    Draw();
}

void TestModule::DrawCircle()
{
    float fx = 0.0;
    float fy = 0.0;

    int nDegrees = 0;

    // Use a triangle fan with 36 tris for the circle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0, 0.0);
    for(nDegrees = 0; nDegrees < 360; nDegrees+=10)
    {
        fx = sin((float)nDegrees*PI/180);
        fy = cos((float)nDegrees*PI/180);
        glVertex2f(fx, fy);
    }
    glVertex2f(0.0f, 1.0f);
    glEnd();
}

void TestModule::Draw()
{
    int32s x = mMouseX;
    int32s y = mMouseY;

    float fRightX = 0.0;
    float fRightY = 0.0;
    float fLeftX = 0.0;
    float fLeftY = 0.0;

    int nLtEyePosX = (mWidth)/2 - (((mWidth)/2) / 3);
    int nLtEyePosY = (mHeight)/2;
    int nRtEyePosX = (mWidth)/2 + (((mWidth)/2) / 3);
    int nRtEyePosY = (mHeight)/2;

    double fLtVecMag = 100;
    double fRtVecMag = 100;

    // Use the eyeball width as the minimum
    double fMinLength =  (0.1 * ((mWidth)/2));

    // Calculate the length of the vector from the eyeball
    // to the mouse pointer
    fLtVecMag = sqrt( pow((double)(x - nLtEyePosX), 2.0) +
                      pow((double)(y - nLtEyePosY), 2.0));

    fRtVecMag =  sqrt( pow((double)(x - nRtEyePosX), 2.0) +
                       pow((double)(y - nRtEyePosY), 2.0));

    // Clamp the minimum magnatude
    if (fRtVecMag < fMinLength)
        fRtVecMag = fMinLength;
    if (fLtVecMag < fMinLength)
        fLtVecMag = fMinLength;

    // Normalize the vector components
    fLeftX = (float)(x - nLtEyePosX) / fLtVecMag;
    fLeftY = -1.0 * (float)(y - nLtEyePosY) / fLtVecMag;
    fRightX = (float)(x - nRtEyePosX) / fRtVecMag;
    fRightY = -1.0 * ((float)(y - nRtEyePosY) / fRtVecMag);

    glClear(GL_COLOR_BUFFER_BIT);

    // Clear matrix stack
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw left eyeball
    glColor3f(1.0, 1.0, 1.0);
    glScalef(0.20, 0.20, 1.0);
    glTranslatef(-1.5, 0.0, 0.0);
    DrawCircle();

    // Draw black
    glColor3f(0.0, 0.0, 0.0);
    glScalef(0.40, 0.40, 1.0);
    glTranslatef(fLeftX, fLeftY, 0.0);
    DrawCircle();

    // Draw right eyeball
    glColor3f(1.0, 1.0, 1.0);
    glLoadIdentity();
    glScalef(0.20, 0.20, 1.0);
    glTranslatef(1.5, 0.0, 0.0);
    DrawCircle();

    // Draw black
    glColor3f(0.0, 0.0, 0.0);
    glScalef(0.40, 0.40, 1.0);
    glTranslatef(fRightX, fRightY, 0.0);
    DrawCircle();

    // Clear matrix stack
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw Nose
    glColor3f(0.5, 0.0, 0.7);
    glScalef(0.20, 0.20, 1.0);
    glTranslatef(0.0, -1.5, 0.0);

    glBegin(GL_TRIANGLES);
    glVertex2f(0.0, 1.0);
    glVertex2f(-0.5, -1.0);
    glVertex2f(0.5, -1.0);
    glEnd();
}

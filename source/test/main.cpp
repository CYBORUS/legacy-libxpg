#include <XPG/Display.hpp>

#include "FancyTestModule.h"

#include <iostream>
using namespace std;

/// run test code
int main(int argc, char** argv)
{
    XPG::Context::Parameters p;
    //p.legacyContext = true;
    p.context.vMajor = 3;
    p.context.vMinor = 2;

    XPG::Context c;
    c.create(p);

    cout << "context version " << c.details.context.vMajor << '.'
        << c.details.context.vMinor << endl;

    cout << "shader version " << c.details.shader.vMajor << '.'
        << c.details.shader.vMinor << endl;

    {
        FancyTestModule ftm(c.details.context.vMajor);

        if (c.details.legacyContext)
        {
            c.setWindowTitle("XPG OpenGL Legacy");
            c.setIconTitle("XPG OpenGL Legacy");
        }
        else
        {
            c.setWindowTitle("XPG OpenGL 3");
            c.setIconTitle("XPG OpenGL 3");
        }

        c.runModule(&ftm);

        // ftm dies here, properly disposing of various OpenGL resources
    }

    c.destroy();

    return 0;
}

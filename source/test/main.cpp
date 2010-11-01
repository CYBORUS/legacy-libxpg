#include <XPG/Network.hpp>
#include <XPG/Thread.hpp>
#include <XPG/Mutex.hpp>
#include <XPG/Timer.hpp>
#include <XPG/Display.hpp>

#include "TestModule.h"
#include "FancyTestModule.h"

#include <iostream>
using namespace std;

class MyTask : public XPG::Task
{
    public:
        virtual void run(XPG::StopFlag stopRequested)
        {
            while (!stopRequested)
            {
                XPG::Idle(1000);
                cout << "Greetings from the task!" << endl;
            }
        }
};

/// run test code
int main(int argc, char** argv)
{
//    MyTask task;
//    XPG::Thread t;
//    cout << "main is starting the task..." << endl;
//    t.start(&task);
//    XPG::Idle(3500);
//    cout << "main is stopping the task..." << endl;
//    t.stop();
//    t.wait();

    cout << sizeof(float) << ' ' << sizeof(mat4f) << endl;

    XPG::Context c;
    c.create();

    if (c.details().legacyContext)
    {
        c.setWindowTitle("XPG OpenGL Legacy");
        c.setIconTitle("XPG OpenGL Legacy");
        TestModule tm;
        c.setKeyboardListener(&tm);
        c.setMouseListener(&tm);
        c.setWindowListener(&tm);
        c.runModule(&tm);
    }
    else
    {
        c.setWindowTitle("XPG OpenGL 3");
        c.setIconTitle("XPG OpenGL 3");
        FancyTestModule ftm;
        c.setKeyboardListener(&ftm);
        c.setMouseListener(&ftm);
        c.setWindowListener(&ftm);
        c.runModule(&ftm);
    }

    c.destroy();

    return 0;
}

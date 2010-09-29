/**
 *  This exists for the sole purpose of running tests. It should contain
 *  absolutely no OS-specific code (without proper #ifdef wrappers, anyway).
 */

#include <XPG/Network.hpp>
#include <XPG/Thread.hpp>
#include <XPG/Mutex.hpp>
#include <XPG/Timer.hpp>
#include <XPG/Display.hpp>

#include "TestModule.h"

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <iostream>
using namespace std;

#define PORT 4211

XPG::Mutex BigMutex;

class MyThread : public XPG::Thread
{
    public:
        MyThread() {}
        virtual ~MyThread() {}

        virtual void run()
        {
            srand(time(NULL));
            while (!stopRequested())
            {
                cout << XPG::GetTicks() << " -- locking..." << endl;
                BigMutex.lock();
                cout << XPG::GetTicks() << " -- locked!" << endl;
                XPG::Idle(rand() % 1000);
                BigMutex.unlock();
                cout << XPG::GetTicks() << " -- unlocked!" << endl;
                XPG::Idle(rand() % 1000);
            }
        }
};

void client()
{
    XPG::NetworkStart();
    SOCKET sock;
    struct sockaddr_in sa;
    int bytes_sent, buffer_length;
    char buffer[200];

    buffer_length = snprintf(buffer, sizeof buffer, "Hello World!") + 1;
    printf("buffer_length = %d\n", buffer_length);

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == sock) /* if socket failed to initialize, exit */
    {
        printf("Error Creating Socket");
        exit(1);
    }

    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(0x7F000001);
    sa.sin_port = htons(PORT);

    bytes_sent = sendto(sock, buffer, buffer_length, 0,(struct sockaddr*)&sa,
                        sizeof sa);
    if (bytes_sent < 0)
        printf("Error sending packet: %s\n", strerror(errno));

    closesocket(sock); /* close the socket */
    XPG::NetworkEnd();
}

void server()
{
    XPG::NetworkStart();
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in sa;
    char buffer[2048];
    ssize_t recsize;
    socklen_t fromlen;

    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) == SOCKET_ERROR)
    {
        perror("error bind failed");
        closesocket(sock);
        exit(1);
    }

    for (;;)
    {
        printf ("recv test....\n");
        recsize = recvfrom(sock, buffer, 2048, 0, (struct sockaddr *)&sa, &fromlen);
        if (recsize < 0)
        {
            //fprintf(stderr, "%s\n", strerror(errno));
            cerr << strerror(errno) << endl;
            //cerr << WSAGetLastError() << endl;
        }
        cout << "recsize: " << recsize << endl;
        XPG::Idle(1000);
        if (recsize > 0) cout << "datagram: " << buffer << endl;
    }

    XPG::NetworkEnd();
}

/// run test code
int main(int argc, char** argv)
{
    XPG::TimerStart();
    cout << XPG_OS_STRING << endl;
    if (argc > 1)
    {
        switch (argv[1][0])
        {
            case 'c':
                client();
                break;
            case 's':
                server();
                break;
            default:
            {}
        }
    }


//    MyThread mt;
//    mt.runConcurrently();
//    XPG::Idle(4000);
//    cout << "main locking" << endl;
//    BigMutex.lock();
//    cout << "main locked" << endl;
//    XPG::Idle(4000);
//    BigMutex.unlock();
//    cout << "main unlocked" << endl;
//    XPG::Idle(4000);
//    mt.stop();
//    cout << "Waiting..." << endl;
//    mt.wait();

    vec2f v;
    v[0] = 5.5f;
    v[1] = 9.9f;
    vec2f w(v);
    vec3f a;
    vec4f b;
    cout << v << endl;
    cout << w << endl;

    XPG::Context c;
    c.create(800, 600, 32);
    TestModule tm;
    c.runModule(&tm);

    return 0;
}

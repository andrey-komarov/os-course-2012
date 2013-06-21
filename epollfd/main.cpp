#include "EpollFD.h"

#include <iostream>

using namespace std;

int main()
{
    Buffer buf(4);
    EpollFD poll;
    //cerr << &poll << " poll\n";
    rcont rd;
    wcont wd;
    int a;
    //cerr << "&rd=" << &rd << ", &wd=" << &wd << "\n";
    wd = [&a, &rd, &wd](EpollFD* epoll, int fd, Buffer& buf, int w)
    {
        //cerr << epoll << " epoll\n";
        //cerr << "wd: &rd=" << &rd << ", &wd=" << &wd << "\n";
        if (w != 0)
            epoll->aread(0, buf, rd);
    };
    rd = [&a, &rd, &wd](EpollFD* epoll, int fd, Buffer& buf, int r)
    {
        //cerr << "rd: &rd=" << &rd << ", &wd=" << &wd << "\n";
        if (r != 0)
            epoll->awrite(1, buf, wd);
    };
    poll.aread(0, buf, rd);
    while (true)
        poll.waitcycle();
}

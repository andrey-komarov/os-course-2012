#include "EpollFD.h"

#include <iostream>

using namespace std;

int main()
{
    Buffer buf(4);
    EpollFD poll;
    //cerr << &poll << " poll\n";
    std::function<void(EpollFD*, int, Buffer&, int)> cont 
        = [&poll, &cont](EpollFD* epoll, int fd, Buffer& buf, int rd)
        {
            if (rd == 0)
            {
                cerr << "Done\n";
            }
            else
            {
                buf.drop(buf.readAvaliable());
                cerr << "Read " << rd << " bytes\n";
                epoll->aread(0, buf, cont); // WTF crash here
            }
        };

    poll.aread(0, buf, cont);
    while (true)
        poll.waitcycle();
}

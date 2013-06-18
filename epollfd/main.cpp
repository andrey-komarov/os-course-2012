#include "EpollFD.h"

#include <iostream>

using namespace std;

int main()
{
    Buffer buf(4);
    EpollFD poll;
    std::function<void(EpollFD*, int, Buffer&, int)> cont 
        = [&cont](EpollFD* epoll, int fd, Buffer& buf, int rd)
        {
            if (rd == 0)
            {
                cerr << "Done\n";
            }
            else
            {
                cerr << "Read " << rd << " bytes\n";
                buf.drop(buf.readAvaliable());
                epoll->aread(0, buf, cont); // WTF how it's possible?
            }
        };

    poll.aread(0, buf, cont);
    while (true)
        poll.waitcycle();
}

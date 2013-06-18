#include "EpollFD.h"

#include <unistd.h>
#include <sys/epoll.h>

#include <exception>

EpollFD::EpollFD()
    : epfd(epoll_create(1))
{
    if (epfd < 0)
        throw std::runtime_error("epoll create error");
}

EpollFD::~EpollFD()
{
    alive.clear();
    if (close(epfd) < 0)
        throw std::runtime_error("epoll close");
}

void EpollFD::aread(int fd, Buffer& buf, rcont cont)
{
    scont newCont = [fd, &buf, this, cont]()
    {
        int n = read(fd, buf.writeTo(), buf.writeAvaliable());
        buf.peek(n);
        cont(this, fd, buf, n);
    };
//    ASyncOperation op;
}

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


void EpollFD::subscribe(int fd, uint32_t event, scont cont)
{
    uint32_t& currentEvents = events[fd];
    if (currentEvents & event)
        throw std::runtime_error("adding already existing action");
    currentEvents |= event;
    actions[{fd, event}] = cont;
    epoll_event e;
    e.events = event;
    e.data.fd = fd;
    int op = currentEvents == event ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    if (epoll_ctl(epfd, op, fd, &e) < 0)
        throw std::runtime_error("epoll_ctl");
}

void EpollFD::unsubscribe(int fd, uint32_t event)
{
    uint32_t& currentEvents = events[fd];
    if ((currentEvents & event) != event)
        throw std::runtime_error("removing non-existing action");
    currentEvents ^= event;
    actions.erase({fd, event});
    epoll_event e;
    e.events = currentEvents;
    e.data.fd = fd;
    int op = currentEvents == 0 ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
    if (epoll_ctl(epfd, op, fd, &e) < 0)
        throw std::runtime_error("epoll_ctl");
}

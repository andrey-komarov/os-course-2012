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
    ASyncOperation op(this, fd, EPOLLIN, nullptr);
    ASyncOperation** me = op.getPthis();
    scont newCont = [me, fd, &buf, this, cont]()
    {
        int n = read(fd, buf.writeTo(), buf.writeAvaliable());
        buf.peek(n);
        alive.erase(**me);
        cont(this, fd, buf, n);
    };
    op.setCont(newCont);

    alive.insert(std::move(op));
}

void EpollFD::waitcycle()
{
    epoll_event tmp[10];
    int n = epoll_wait(epfd, tmp, sizeof(tmp) / sizeof(epoll_event), -1);
    if (n < 0)
        throw std::runtime_error("epoll_wait");
    for (int i = 0; i < n; i++)
    {
        epoll_event& cur = tmp[i];
        for (auto action : {EPOLLIN, EPOLLOUT})
            actions[{static_cast<int>(cur.data.fd), action}]();
    }
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

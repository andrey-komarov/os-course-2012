#include "EpollFD.h"

ASyncOperation::ASyncOperation(EpollFD* root, int fd, int events, scont cont)
    : root(root), fd(fd), events(events), cont(cont), pthis(new ASyncOperation*)
{
    root->subscribe(fd, events, cont);
}

ASyncOperation& ASyncOperation::operator=(ASyncOperation&& op)
{
    std::swap(root, op.root);
    std::swap(valid, op.valid);
    std::swap(fd, op.fd);
    std::swap(events, op.events);
    std::swap(cont, op.cont);
    *pthis = this;
    op.valid = false;
    return *this;
}

ASyncOperation::ASyncOperation(ASyncOperation&& op)
{
    *this = std::move(op);
}

ASyncOperation::~ASyncOperation()
{
    if (valid)
    {
        delete pthis;
        root->unsubscribe(fd, events);
    }
}

bool ASyncOperation::operator<(const ASyncOperation& op) const
{
    if (fd != op.fd)
        return fd < op.fd;
    return events < op.events;
}

bool ASyncOperation::operator==(const ASyncOperation& op) const
{
    if (fd != op.fd)
        return false;
    if (events != op.events)
        return false;
    return true;
}

void ASyncOperation::setCont(scont c)
{
    cont = c;
}

ASyncOperation** ASyncOperation::getPthis()
{
    return pthis;
}

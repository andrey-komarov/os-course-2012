#include "EpollFD.h"

#include <assert.h>

#include <iostream>
using namespace std;

ASyncOperation::ASyncOperation(EpollFD* root, int fd, int events, scont cont)
    : root(root), valid(true), fd(fd), events(events), cont(cont), pthis(new ASyncOperation*)
{
    *pthis = this;
    //cerr << "ASyncOperation::ASyncOperation()\n";
    //cerr << "... pthis = " << pthis << "\n";
    root->subscribe(fd, events, cont);
}

ASyncOperation& ASyncOperation::operator=(ASyncOperation&& op)
{
    //cerr << "ASyncOperation::operator=(&&)\n";
    std::swap(root, op.root);
    std::swap(valid, op.valid);
    std::swap(fd, op.fd);
    std::swap(events, op.events);
    std::swap(cont, op.cont);
    pthis = op.pthis;
    //cerr << "*pthis : " << *pthis << " -> " << this << "\n";
    *pthis = this;
    op.valid = false;
    return *this;
}

ASyncOperation::ASyncOperation(ASyncOperation&& op)
{
    //cerr << "ASyncOperation::ASyncOperation(&&)\n";
    *this = std::move(op);
}

ASyncOperation::~ASyncOperation()
{
    //cerr << "ASyncOperation::~ASyncOperation()\n";
    if (valid)
    {
        //cerr << "... valid\n";
        delete pthis;
        root->unsubscribe(fd, events);
    }
}

bool ASyncOperation::operator<(const ASyncOperation& op) const
{
    assert (valid || op.valid);
    if (fd != op.fd)
        return fd < op.fd;
    if (events != op.events)
        return events < op.events;
    return valid < op.valid;
}

bool ASyncOperation::operator==(const ASyncOperation& op) const
{
    if (valid != op.valid)
        return false;
    if (fd != op.fd)
        return false;
    if (events != op.events)
        return false;
    return true;
}

void ASyncOperation::setCont(scont c)
{
    cont = c;
    root->actions[{fd, events}] = c;
}

ASyncOperation** ASyncOperation::getPthis()
{
    return pthis;
}

std::ostream& operator<<(std::ostream& out, const ASyncOperation& op)
{
    if (!op.valid)
        out << "invalid";
    else
    {
        assert (*op.pthis == &op);
        out << "[fd=" << op.fd << ", events=" << op.events << "]";
    }
    return out;
}

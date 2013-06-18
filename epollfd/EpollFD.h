#ifndef EPOLLFD_H
#define EPOLLFD_H

#include "Buffer.h"

#include <functional>
#include <set>
#include <map>

struct EpollFD;
struct ASyncOperation;
typedef std::function<void(EpollFD*, int fd, Buffer& buf, int rd)> rcont;
typedef std::function<void(void)> scont;

struct ASyncOperation
{
    ASyncOperation(EpollFD* root, int fd, int events, scont cont);

    ASyncOperation(const ASyncOperation&) = delete;
    ASyncOperation(ASyncOperation&&);
    ASyncOperation& operator=(const ASyncOperation&) = delete;
    ASyncOperation& operator=(ASyncOperation&&);

    ~ASyncOperation();

    bool operator<(const ASyncOperation&) const;
    bool operator==(const ASyncOperation&) const;
private:
    EpollFD* root;
    bool valid;
    int fd;
    int events;
    scont cont;
};

struct EpollFD
{
    EpollFD();
    EpollFD(const EpollFD&) = delete;
    EpollFD(EpollFD&&) = delete;
    EpollFD& operator=(const EpollFD&) = delete;
    EpollFD& operator=(EpollFD&&) = delete;
    ~EpollFD();

    void aread(int fd, Buffer& buf, rcont cont);

private:
    std::set<ASyncOperation> alive;
    int epfd;
};

#endif // EPOLLFD_H


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

    void setCont(scont); // :'(
    ASyncOperation** getPthis();

    bool operator<(const ASyncOperation&) const;
    bool operator==(const ASyncOperation&) const;
private:
    EpollFD* root;
    bool valid;
    int fd;
    int events;
    scont cont;
    ASyncOperation **pthis;
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

    void waitcycle();
private:
    void subscribe(int fd, uint32_t events, scont cont);
    void unsubscribe(int fd, uint32_t events);

    std::set<ASyncOperation> alive;
    std::map<int, uint32_t> events;
    std::map<std::pair<int, uint32_t>, scont> actions;
    int epfd;

    friend class ASyncOperation;
};

#endif // EPOLLFD_H


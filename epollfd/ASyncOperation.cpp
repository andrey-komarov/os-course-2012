#include "EpollFD.h"

ASyncOperation::ASyncOperation(EpollFD* root, int fd, int events, scont cont)
    : root(root), fd(fd), events(events), cont(cont)
{}

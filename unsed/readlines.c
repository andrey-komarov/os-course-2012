#include "readlines.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned char uchar;

typedef struct
{
    int fildes;
    int bufsize;
    uchar* buf;
    int bufpos;
    int read_len;
} BR;

typedef struct RL
{
    uchar* buf;
    int fildes;
    int bufsize;
    BR* reader;
    int skipping;
} RL;

size_t rl_max_size(RL* rl)
{
    return rl->bufsize;
}

// -1 --- error
// -2 --- EOF
int read_char(BR* br)
{
    if (br->bufpos == br->read_len)
    {
        br->bufpos = 0;
        br->read_len = read(br->fildes, br->buf, br->bufsize);
        if (br->read_len == 0)
            return -2; // EOF reached
        if (br->read_len == -1)
            return -1; // Error
    }
    return br->buf[br->bufpos++];
}

RL* rl_open(int fd, size_t size)
{
    size++; // for \\n
    RL* rl = malloc(sizeof(RL));
    rl->buf = (uchar*)malloc(size);
    rl->fildes = fd;
    rl->bufsize = size;
    rl->skipping = 0;
    BR* br = malloc(sizeof(BR));
    br->fildes = fd;
    br->bufsize = size;
    br->bufpos = 0;
    br->buf = (uchar*)malloc(size);
    rl->reader = br;
    return rl;
}

int br_close(BR* br)
{
    free(br->buf);
    return 0;
}

int rl_close(RL* rl)
{
    br_close(rl->reader);
    free(rl->buf);
    close(rl->fildes);
    free(rl);
    return 0;
}

int rl_readline(RL *rl, char* buf, size_t buf_size)
{
    int len;
    for (len = 0; len < rl_max_size(rl); len++)
    {
        int next_char = read_char(rl->reader);
        if (next_char == -1)
            return -1; // Error
        if (next_char == -2)
            return 0; // EOF
        rl->buf[len] = next_char;
        if (next_char == '\n')
        {
            len++;
            break;
        }
    }
    if (len == rl_max_size(rl) && rl->buf[len - 1] != '\n')
    {
        rl->skipping = 1;
        return -3; // Line is too long to fit to buffer
    }
    else
    {
        if (rl->skipping)
        {
            rl->skipping = 0;
            return -3; // Long line last piece read
        }
        else
        {
            memcpy(buf, rl->buf, len);
            buf[len] = 0; // 0-terminated string
            return len + 1; // success
        }

    }
}

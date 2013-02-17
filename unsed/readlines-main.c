#include "readlines.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: ./readlines <length>\n");
        return 1;
    }
    int length = atoi(argv[1]);
    if (length < 0 || length > (1<<30))
    {
        printf("Incorrect length: %d", length);
        return 2;
    }
    struct RL* rl = rl_open(0, length);
    char* c = malloc(100);
    int t;
    while ((t = rl_readline(rl, c, 100)))
    {
        if (t > 0)
            printf("%s", c);
    }
    return 0;
}

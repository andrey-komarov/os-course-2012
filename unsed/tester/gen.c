#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>

const char* usage1 = "Usage: ./gen <minlen> <maxlen>\n";
const char* usage2 = "Lower bound is bigger than upper bound\n";

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        write(2, usage1, strlen(usage1));
        return 1;
    }
    int from = atoi(argv[1]);
    int to = atoi(argv[2]);
    if (from > to)
    {
        write(2, usage2, strlen(usage2));
        return 2;
    }
    int urandom = open("/dev/urandom", O_RDONLY);
    while (1)
    {
        int rnd;
        read(urandom, &rnd, sizeof(rnd));
        int length = from + (rnd % (to - from));
        int i;
        for (i = 0; i < length; i++)
        {
            while (1)
            {
                unsigned char rndch;
                read(urandom, &rndch, sizeof(rndch));
                if (rndch == '\n' || rndch < 32 || rndch >= 128)
                    continue;
                write(1, &rndch, 1);
                break;
            }
        }
        char ch = '\n';
        write(1, &ch, 1);
    }
}

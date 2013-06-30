#include <stdio.h>
#include <stdlib.h>

int main()
{
    int* a = malloc(sizeof(int));
    *a = 2;
    printf("hello");
    return 0;
}

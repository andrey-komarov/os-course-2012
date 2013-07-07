#include <stdio.h>
#include <stdlib.h>

int main()
{
    int* a = malloc(sizeof(int));
    *a = 2;
    free(a);
    printf("hello");
    return 0;
}

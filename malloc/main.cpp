#include "malloc.h"

#include <iostream>

using namespace std;

int main()
{
    void* ptr = malloc(10);
    *(int*)ptr = 2;
    cout << "Hello!\n";
}

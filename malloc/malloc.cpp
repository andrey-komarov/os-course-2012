#include "malloc.h"

#include <iostream>
using std::cerr;

void* malloc(size_t size) noexcept
{
    cerr << "malloc(" << size << ")\n";
    return NULL;
}

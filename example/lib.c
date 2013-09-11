#include "lib.h"

#include <stdlib.h>
#include <unistd.h>

int returns_1(void)
{
    return 1;
}

int crash(void)
{
    int * i;
    *i = 19;
    return *i;
}

int sleeps_5(void)
{
    sleep(5 * 1000);

    return 1;
}


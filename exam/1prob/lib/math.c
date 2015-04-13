#include "math.h"

float pow(float base, int power)
{
    int i;
    int result = base;

    for(i = 1; i < power; i++)
        result *= base;

    if(power < 0)
        return 1 / result;
    if(power == 0)
        return 1;
    if(power > 0)
        return result;
}


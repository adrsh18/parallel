#include "utils.h"
#include <math.h>

int numRecursiveHalvingPasses(int nProcs)
{
    return (int) ceil((log(nProcs)/log(2)));
}

int power(int x, int y)
{
    return (int) pow(x,y);
}

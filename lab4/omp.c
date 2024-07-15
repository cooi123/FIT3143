#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int main()
{

    int i = 3;
    printf("%d", i ^ 3);
#pragma omp parallel
    {
#pragma omp for
        for (i = 0; i < 10; ++i)
        {
            printf(" %d", i);
        }
    }

    return 0;
}
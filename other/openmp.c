#include <stdio.h>
#include <omp.h>
int main()
{
    int i;
    printf("hello world\n");

#pragma omp parallel for
    for (i = 0; i < 60; i++)
        printf("Iter:%d\n", i);

    printf("goodbye world\n");
}

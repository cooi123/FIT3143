#include <stdio.h>
#include <omp.h>
#include <time.h>
#define M 4
#define N 5
int main()
{

    int a[M][N] =
        {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}, {16, 17, 18, 19, 20}};
    int b[M][N] =
        {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}, {16, 17, 18, 19, 20}};
    int c[M][N] = {0};
    int i, j;

    struct timespec start, end;
    double time_taken;

    clock_gettime(CLOCK_MONOTONIC, &start);
#pragma omp parallel for
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
        {
            c[i][j] = a[i][j] * b[i][j];
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    Page 5 printf("Overall time (s): %lf\n", time_taken);

    printf("Values of C:\n");
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("%d\t", c[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}

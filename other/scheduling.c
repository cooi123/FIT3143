#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>
#define THREADS 6
#define N 18
int main()
{
    int i;
    struct timespec start, end;
    double time_taken;
    clock_gettime(CLOCK_MONOTONIC, &start);
#pragma omp parallel for schedule(dynamic) num_threads(THREADS)
    for (i = 0; i < N; i++)
    {
        sleep(i);
        printf("Thread %d has completed iteration %d.\n",
               omp_get_thread_num(), i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("overall time(s):%lf\n ", time_taken);
    /* all threads done */

    printf("All done!\n");

    return 0;
}

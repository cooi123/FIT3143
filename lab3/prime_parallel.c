#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#define THREAD_NUM 6

int num;
int *arr;
void *CalculatePrimeThreadFunc(void *pArg);
int isPrime(int n)
{
    for (int i = 2; i <= sqrt(n); i++)
    {
        if (n % i == 0)
        {
            return 0;
        }
    }

    return 1;
}
int main()
{
    pthread_t tid[THREAD_NUM];
    int threadNum[THREAD_NUM];
    num = 5;
    printf("Enter number: ");
    scanf("%d", &num);
    struct timespec start, end, startComp, endComp;
    clock_gettime(CLOCK_MONOTONIC, &start);
    arr = malloc(sizeof(int) * num + 1);
    clock_gettime(CLOCK_MONOTONIC, &startComp);
    // fork
    pthread_t temp;
    for (int i = 0; i < THREAD_NUM; i++)
    {
        threadNum[i] = i;
        pthread_create(&tid[i], 0, CalculatePrimeThreadFunc, &threadNum[i]);
    }

    // join
    for (int i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(tid[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &endComp);

    FILE *fptr;

    fptr = fopen("primes_parallel.txt", "w");
    if (fptr == NULL)
    {
        exit(1);
    }

    for (int i = 2; i <= num; i++)
    {
        if (arr[i] == 1)
        {
            fprintf(fptr, "%d\n", i);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (endComp.tv_sec - startComp.tv_sec) * 1e9;
    time_taken = (time_taken + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9;
    printf("Computational time only(s): %lf\n", time_taken);

    free(arr);
    fclose(fptr);
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("Overall time (Including read, product and write)(s): %lf\n", time_taken);
}

void *CalculatePrimeThreadFunc(void *pArg)
{
    int id = *((int *)pArg);
    int dis = num / THREAD_NUM;
    int rem = num % THREAD_NUM;
    int start = dis * id;
    int end = id == THREAD_NUM - 1 ? start + dis + rem : start + dis;
    printf("thread id %d, start index: %d, end inex: %d\n", id, start, end);
    for (int i = start; i < end; i++)
    {
        if (isPrime(i) == 1)
        {
            arr[i] = 1;
        }
        else
        {
            arr[i] = 0;
        }
    }
}

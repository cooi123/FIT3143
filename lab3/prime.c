#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int num;
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
    num = 5;
    printf("Enter number: ");
    scanf("%d", &num);
    struct timespec start, end, startComp, endComp;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int *arr = malloc(sizeof(int) * num + 1);
    clock_gettime(CLOCK_MONOTONIC, &startComp);
    for (int i = 0; i <= num; i++)
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
    clock_gettime(CLOCK_MONOTONIC, &endComp);

    FILE *fptr;

    fptr = fopen("primes.txt", "w");
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

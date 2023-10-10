#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#define k 3
// int incoming_car_simulator()
// {
//     int rand_val = rand() % 2;
// }
int main()
{
    //     int arr[k] = {0};
    //     int sum = 0;
    //     for (int i = 0; i < 100; i++)
    //     {
    //         // #pragma omp parallel for
    //         // for (int j=0; j<k; j++){
    //         //     int val = incoming_car_simulator();
    //         //     arr[j] = val;
    //         // }

    // #pragma omp parallel for reduction(+ : sum)
    //         for (int i = 0; i < k; i++)
    //         {
    //             int val = incoming_car_simulator();
    //             arr[i] = val;
    //             sum += arr[i];
    //         }
    //         printf("%d\n", sum);
    //         sum = 0;
    //     }
    time_t now = time(NULL);
    // struct tm *ptm = gmtime(&now);
    // int day = ptm->tm_mday;
    // printf("The time is: %02d:%02d:%02d\n", ptm->tm_hour,
    //        ptm->tm_min, ptm->tm_sec);
    printf("Current day %d", now);
    return 0;
}

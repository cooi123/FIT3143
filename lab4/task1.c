#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
void write_int_arr_file(int *arr, int size, char *file_name);
int *read_file1(char *file_name, int size);

int main(int arc, char *argv[])
{
    int p = 4, size = 10;
    int arr[size];
    int i;
#pragma omp parallel for num_threads(p)
    for (i = 0; i < size; i++)
    {
        srand(i);
        int tid = omp_get_thread_num();
        int r = rand() % 10;
        arr[i] = r;
        printf("index %d", i);
        printf("thread id %d: rand: %d\n", tid, r);
    }
    write_int_arr_file(arr, size, "display.txt");
    int *val = read_file1("display.txt", size);
    int occurance[10] = {0};
    for (int i = 0; i < size; i++)
    {
        int val = arr[i];
        occurance[val]++;
    }
    int win_sum = 0;
#pragma omp parallel for reduction(+ : win_sum)
    for (int i = 0; i < 10; i++)
    {

        if (occurance[i] > 1)
        {
            win_sum++;
        }
    }
}

void write_int_arr_file(int *arr, int size, char *file_name)
{
    FILE *ptr;
    ptr = fopen(file_name, "w");
    for (int i = 0; i < size; i++)
    {
        fprintf(ptr, "%d\n", arr[i]);
    }
    fclose(ptr);
}

void write_file(int val, char *file_name)
{
    FILE *ptr;
    ptr = fopen(file_name, "w");
    fprintf("%d", val);
    fclose(ptr);
}

int *read_file1(char *file_name, int size)
{
    FILE *ptr;
    ptr = fopen(file_name, "w");
    int *val = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
    {
        fscanf(ptr, "%d", &val[i]);
    }
    fclose(ptr);
    return val;
}
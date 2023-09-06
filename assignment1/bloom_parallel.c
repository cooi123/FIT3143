#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "read_file.h"
#define MAX_FP_RATE 0.05
#define HASH_FUNCTIONS 5
unsigned int jenkins_one_at_a_time_hashing(char *string, int m);

const int prime_nums[HASH_FUNCTIONS] = {
    31,
    193,
    389,
    941, 1193};
int isInArray(char *target, char *bit_array, int bit_array_size);
int read_from_files(char **file_paths, int num_files, char ****pUniqueWord, int **unique_length_files)
{
    char ***all_files_unique_word = (char ***)malloc(sizeof(char **) * num_files);
#pragma omp parallel for
    for (int i = 1; i <= num_files; i++)
    {
        int file_index = i - 1;
        char *file = file_paths[i];
        int number_lines = countLineInFiles(file);
        char **all_words_file = (char **)malloc(sizeof(char *) * number_lines);
        if (readFileToArray(file, number_lines, &all_words_file) == 0)
        {
            perror("error reading words to array");
            return -1;
        }

        *(unique_length_files + file_index) = findUniqueWord(all_words_file, number_lines, &all_files_unique_word[file_index]);
        printf("%s has number of lines %d has %d of unique words\n", file, number_lines, *(unique_length_files + file_index));
    }
    *pUniqueWord = all_files_unique_word;
    return 1;
}

int main(int argc, char **argv)
{
    if (!argc || argc <= 1)
    {
        perror("please provide file paths");
        return EXIT_FAILURE;
    }
    int num_files = argc - 1;
    printf("number of files %d\n", num_files);
    char ***unique_words_files;
    int unique_length_files[num_files];
    char *bit_arrays[num_files];
    struct timespec start, end, startReading, endReading, startFindingUnique, endFindingUnique, startHashComp, endHashComp;
    double time_taken;
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &startReading);

    if (read_from_files(argv, num_files, &unique_words_files, &unique_length_files) == 0)
    {
        perror("error getting words from file");
        return EXIT_FAILURE;
    }

    clock_gettime(CLOCK_MONOTONIC, &endReading);

    time_taken = (endReading.tv_sec - startReading.tv_sec) * 1e9;
    time_taken = (time_taken + (endReading.tv_nsec - startReading.tv_nsec)) * 1e-9;
    printf("Reading file process time(s): %lf\n", time_taken);

    clock_gettime(CLOCK_MONOTONIC, &startHashComp);
    for (int i = 0; i < num_files; i++)
    {
        insertingHashedValues(unique_length_files[i], unique_words_files[i], &bit_arrays[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &endHashComp);
    time_taken = (endHashComp.tv_sec - startHashComp.tv_sec) * 1e9;
    time_taken = (time_taken + (endHashComp.tv_nsec - startHashComp.tv_nsec)) * 1e-9;
    printf("Inserting process time(s): %lf\n", time_taken);

    // Get the clock current time again
    // Subtract end from start to get the CPU time used.
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("Total Process time(s): %lf\n", time_taken);

    // for (int i = 0; i < num_files; i++)
    // {
    //     printf("%s", bit_arrays[i]);
    // }
    return 0;
}

int insertingHashedValues(int unique_words_length, char **unique_words, char **p_Bit_array)
{
    int bit_array_size = -(unique_words_length * log(MAX_FP_RATE)) / pow(log(2), 2);
    char *bit_array = (char *)malloc(bit_array_size + 1 * sizeof(char));
    for (int i = 0; i < bit_array_size; i++)
    {
        bit_array[i] = '0';
    }
    bit_array[bit_array_size] = '\0';
#pragma omp parallel for collapse(2)
    for (int i = 0; i < unique_words_length; i++)
    {

        for (int p_i = 0; p_i < HASH_FUNCTIONS; p_i++)
        {
            unsigned int hash_val = jenkins_one_at_a_time_hashing(unique_words[i], prime_nums[p_i]);
            // printf("hash index:%u", hash_val % m);
            bit_array[hash_val % bit_array_size] = '1';
        }
    }
    *p_Bit_array = bit_array;
    return EXIT_SUCCESS;
}

int isInArray(char *target, char *bit_array, int bit_array_size)
{
    for (int p_i = 0; p_i < HASH_FUNCTIONS; p_i++)
    {
        unsigned int hash_val = jenkins_one_at_a_time_hashing(target, prime_nums[p_i]);
        char bool = bit_array[hash_val % bit_array_size];
        if (bool == '0')
        {
            return 0;
        }
    }
    return 1;
}

/**
 *
 */
unsigned int jenkins_one_at_a_time_hashing(char *string, int m)
{

    unsigned int hash_value = 0;

    for (int i = 0; i < strlen(string); i++)
    {
        hash_value += string[i];
        hash_value += (hash_value << 10);
        hash_value ^= (hash_value >> 6);
    }
    hash_value += (hash_value << 3);
    hash_value ^= (hash_value >> 11);
    hash_value += (hash_value << 15);

    return hash_value * m;
}

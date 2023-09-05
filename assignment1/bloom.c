#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
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

int main()
{
    char **unique_words;
    struct timespec start, end, startReading, endReading, startFindingUnique, endFindingUnique, startHashComp, endHashComp;
    double time_taken;

    const int fileLength = 215724;
    char *file = "MOBY_DICK.txt";

    char **allWords = (char **)malloc(fileLength * sizeof(char *));
    // Get current clock time.
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &startReading);

    if (readFileToArray(file, fileLength, &allWords) == 0)
    {
        perror("error reading words to array");
        return -1;
    }
    clock_gettime(CLOCK_MONOTONIC, &endReading);
    time_taken = (endReading.tv_sec - startReading.tv_sec) * 1e9;
    time_taken = (time_taken + (endReading.tv_nsec - startReading.tv_nsec)) * 1e-9;
    printf("Reading file process time(s): %lf\n", time_taken);

    clock_gettime(CLOCK_MONOTONIC, &startFindingUnique);
    int unique_words_length = findUniqueWord(allWords, fileLength, &unique_words);

    clock_gettime(CLOCK_MONOTONIC, &endFindingUnique);
    time_taken = (endFindingUnique.tv_sec - startFindingUnique.tv_sec) * 1e9;
    time_taken = (time_taken + (endFindingUnique.tv_nsec - startFindingUnique.tv_nsec)) * 1e-9;
    printf("Finindg unique words process time(s): %lf\n", time_taken);

    int bit_array_size = -(unique_words_length * log(MAX_FP_RATE)) / pow(log(2), 2);

    printf("unqie words: %d\n", unique_words_length);
    printf("table size: %d\n ", bit_array_size);

    // insert large prime number to multiply
    char *bit_array = (char *)malloc(bit_array_size + 1 * sizeof(char));
    for (int i = 0; i < bit_array_size; i++)
    {
        bit_array[i] = '0';
    }
    bit_array[bit_array_size] = '\0';

    clock_gettime(CLOCK_MONOTONIC, &startHashComp);
    for (int i = 0; i < unique_words_length; i++)
    {

        for (int p_i = 0; p_i < HASH_FUNCTIONS; p_i++)
        {
            unsigned int hash_val = jenkins_one_at_a_time_hashing(unique_words[i], prime_nums[p_i]);
            // printf("hash index:%u", hash_val % m);
            bit_array[hash_val % bit_array_size] = '1';
        }
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

    // printf("%s", bit_array);
    for (int i = 0; i < fileLength; i++)
    {
        free(allWords[i]);
    }
    free(allWords);

    for (int i = 0; i < unique_words_length; i++)
    {
        free(unique_words[i]);
    }
    free(unique_words);
    // while (1)
    // {
    //     printf("enter word:\n");
    //     char input_word[100];
    //     scanf("%s", input_word);
    //     printf("finding word %s\n", input_word);
    //     if (isInArray(input_word, bit_array, bit_array_size) == 0)
    //     {
    //         printf("word not found");
    //     }
    // }
    return 0;
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

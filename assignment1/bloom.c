#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "read_file.h"
#define MAX_FP_RATE 0.05
#define HASH_FUNCTIONS 8
unsigned int jenkins_one_at_a_time_hashing(char *string, int m);
int insertingHashedValues(int unique_words_length, char **unique_words, char **p_Bit_array);
int read_from_files(char **file_paths, int num_files, char ****pUniqueWord, int **unique_length_files);
int isInArray(char *target, char *bit_array, int bit_array_size);
const int prime_nums[HASH_FUNCTIONS] = {
    31,
    193,
    389,
    941, 1193, 2693, 2711, 2731};

/***
 * argv file to read, file to query is always at the end of argument
 */
int main(int argc, char **argv)
{
    if (!argc || argc <= 2)
    {
        perror("please provide read file paths and query file path");
        return EXIT_FAILURE;
    }

    char *query_file_path = argv[argc - 1];
    int num_files = argc - 2;
    printf("number of files %d\n", num_files);
    char ***unique_words_files;

    int *unique_length_files;
    char *bit_arrays[num_files];
    struct timespec start, end, startReading, endReading, startHashComp, endHashComp, startQueryComp, endQueryComp;
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

    FILE *file = fopen(query_file_path, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }
    char queryWord[100];
    int found;
    int falsePostiveNum = 0;
    clock_gettime(CLOCK_MONOTONIC, &startQueryComp);

    // querying line by line- not storing the query words
    while (fscanf(file, "%s %d", queryWord, &found) == 2)
    {
        int files_check = 0;

        for (int i = 0; i < num_files; i++)
        {
            int bit_array_size = strlen(bit_arrays[i]);
            if (isInArray(queryWord, bit_arrays[i], bit_array_size) != found)
            {
                files_check++;
            }
        }
        if (files_check == num_files)
        {
            // printf("%s not found in bit array\n", queryWord);

            falsePostiveNum++;
        }
    }
    printf("false positive number %d", falsePostiveNum);
    fclose(file);

    clock_gettime(CLOCK_MONOTONIC, &endQueryComp);
    time_taken = (endQueryComp.tv_sec - startQueryComp.tv_sec) * 1e9;
    time_taken = (time_taken + (endQueryComp.tv_nsec - startQueryComp.tv_nsec)) * 1e-9;
    printf("Query process time(s): %lf\n", time_taken);

    // Get the clock current time again
    // Subtract end from start to get the CPU time used.
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("Total Process time(s): %lf\n", time_taken);
    // free allocated memory
    for (int i = 0; i < num_files; i++)
    {
        free(unique_words_files[i]);
    }

    free(unique_words_files);
    free(unique_length_files);

    return 0;
}

/***
 * read from files and find the unique word
 * Store unique word in to an array of array string
 * Store unique length of each file in a int array
 * Return 1 if sucessfull
 */
int read_from_files(char **file_paths, int num_files, char ****pUniqueWord, int **unique_length_files)
{
    char ***all_files_unique_word = (char ***)malloc(sizeof(char **) * num_files);
    int *unique_length_files_temp = (int *)malloc(sizeof(int) * num_files);
    for (int i = 1; i <= num_files; i++)
    {
        int file_index = i - 1;
        char *file = file_paths[i];
        int number_lines = countLineInFiles(file);
        char **all_words_file = (char **)malloc(sizeof(char *) * number_lines);
        if (readFileToArray(file, number_lines, &all_words_file) == 0)
        {
            // perror("error reading words to array");
            // return -1;
        }
        unique_length_files_temp[file_index] = findUniqueWord(all_words_file, number_lines, &all_files_unique_word[file_index]);
        printf("%s has number of lines %d has %d of unique words\n", file, number_lines, unique_length_files_temp[file_index]);
    }
    *pUniqueWord = all_files_unique_word;
    *unique_length_files = unique_length_files_temp;
    return 1;
}

/**
 * Input a list of strings and a pointer to the bit array
 * uses k number of hash function to perform bloom insertion
 * return 0 if succesfully insert
 */
int insertingHashedValues(int unique_words_length, char **unique_words, char **p_Bit_array)
{
    int bit_array_size = -(unique_words_length * log(MAX_FP_RATE)) / pow(log(2), 2);
    printf("unique word length %d, hash val size %d", unique_words_length, bit_array_size);
    char *bit_array = (char *)malloc(bit_array_size + 1 * sizeof(char));
    // initalise bit array to 0;
    for (int i = 0; i < bit_array_size; i++)
    {
        bit_array[i] = '0';
    }
    bit_array[bit_array_size] = '\0';
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

/**
 * Querying bloom bit array
 * Input a string and the bit array
 * return 0 if not found, return 1 if found
 */
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
 * input string to be hashed and another prime number to distrube the hash value
 * Hash function modified from wikipedia
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

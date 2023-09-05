#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
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
    int unique_words_length = readUniqueWordFromFile("test.txt", 36, &unique_words);
    int bit_array_size = 100;
    // -unique_words_length * log(MAX_FP_RATE) / pow((2), 2);
    printf("unqie words: %d\n", unique_words_length);
    printf("table size: %d\n ", bit_array_size);

    // insert large prime number to multiply
    char *bit_array = (char *)malloc(bit_array_size + 1 * sizeof(char));
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
    printf("%s\n", bit_array);
    while (1)
    {
        printf("enter word:\n");
        char input_word[100];
        scanf("%s", input_word);
        printf("finding word %s\n", input_word);
        if (isInArray(input_word, bit_array, bit_array_size) == 0)
        {
            printf("word not found");
        }
    }
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

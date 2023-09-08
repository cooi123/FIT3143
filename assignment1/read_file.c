#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "read_file.h"
#include <time.h>

// int main()
// {
//     struct timespec start, end, startReading, endReading, startUnique, endUnique;
//     double time_taken;

//     char **all_words;
//     char **uniqueWords;
//     char *textfiles[] = {"LITTLE_WOMEN.txt", "MOBY_DICK.txt", "SHAKESPEARE.txt", "output.txt"};
//     // int fileSize[] = {195467, 215724, 965466, 528921};
//     clock_gettime(CLOCK_MONOTONIC, &start);
//     clock_gettime(CLOCK_MONOTONIC, &startReading);
//     for (int i = 0; i < 4; i++)
//     {
//         int fileSize = countLineInFiles(textfiles[i]);
//         readFileToArray(textfiles[i], fileSize, &all_words);
//         clock_gettime(CLOCK_MONOTONIC, &endReading);
//         time_taken = (endReading.tv_sec - startReading.tv_sec) * 1e9;
//         time_taken = (time_taken + (endReading.tv_nsec - startReading.tv_nsec)) * 1e-9;
//         printf("Reading process time(s): %lf\n", time_taken);

//         clock_gettime(CLOCK_MONOTONIC, &startUnique);
//         int unique_words_length = findUniqueWord(all_words, 195467, &uniqueWords);
//         printf("%d", unique_words_length);
//         clock_gettime(CLOCK_MONOTONIC, &endUnique);
//         time_taken = (endUnique.tv_sec - startUnique.tv_sec) * 1e9;
//         time_taken = (time_taken + (endUnique.tv_nsec - startUnique.tv_nsec)) * 1e-9;
//         printf("Unique process time(s): %lf\n", time_taken);
//     }

//     clock_gettime(CLOCK_MONOTONIC, &end);
//     time_taken = (end.tv_sec - start.tv_sec) * 1e9;
//     time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
//     printf("Total process time(s): %lf\n", time_taken);
// }
/**
 * count number of new lines in a file
 * Assume that each line only contain one word
 *
 */
int countLineInFiles(char *filePath)
{
    FILE *file;
    file = fopen(filePath, "r");
    int count = 1;
    char c;
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    // Extract characters from file and store in character c
    for (c = getc(file); c != EOF; c = getc(file))
        if (c == '\n') // Increment count if this character is newline
            count++;

    // Close the file
    fclose(file);

    return count;
}

/**
 * loop thorugh the proivded array and use strcomparison to check whether the target word is in the list
 *
 */
int wordInArray(char *word, char **wordArray, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(word, wordArray[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/***
 * Read file in array, requires the file length
 * Assueme each line only has 1 word
 */
int readFileToArray(char *filepath, int fileLength, char ***pAllWords)
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }
    char **allWords = (char **)malloc(fileLength * sizeof(char *));

    char wordBuffer[MAX_WORD_LENGTH];
    for (int i = 0; i < fileLength; i++)
    {
        fscanf(file, "%s", wordBuffer);

        for (int index = 0; index < strlen(wordBuffer); index++)
        {
            wordBuffer[index] = tolower(wordBuffer[index]);
        }
        allWords[i] = strdup(wordBuffer);
    }
    *pAllWords = allWords;
    fclose(file);

    return 1;
}

/***
 * find the unique word in a given array of string by slowing building a unique array.
 */
int findUniqueWord(char **allWords, int fileLength, char ***pWordArray)
{

    int uniqueWordLength = 0;
    char **uniqueWords = (char **)malloc(fileLength * sizeof(char *));
    for (int i = 0; i < fileLength; i++)
    {
        char *temp = allWords[i];
        if (wordInArray(temp, uniqueWords, uniqueWordLength) == 0)
        {
            uniqueWords[uniqueWordLength++] = strdup(temp);
        }
    }
    uniqueWords = realloc(uniqueWords, uniqueWordLength * sizeof(char *));
    *pWordArray = uniqueWords;
    return uniqueWordLength;
}

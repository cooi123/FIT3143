#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "read_file.h"

// int main()
// {
//     // char **unique_words;
//     // int unique_words_length = readUniqueWordFromFile("LITTLE_WOMEN.txt", 195467, &unique_words);
//     // printf("%d", unique_words_length);
//     int line = countLineInFiles("MOBY_DICK.txt");
//     printf("%d words", line);
// }

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

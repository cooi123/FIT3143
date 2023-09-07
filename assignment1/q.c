#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main()
{
    FILE *file = fopen("query.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }
    char word[100];
    int found;
    while (fscanf(file, "%s %d", word, &found) == 2)
    {
        printf("%s %d\n", word, found);
    }
    fclose(file);
    return 1;
}
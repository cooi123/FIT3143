#include <stdio.h>
#include <math.h>
int main()
{
    int i, nc;
    nc = 0;
    i = getchar();
    while (i != EOF)
    {
        nc = nc + 1;
        printf("%c", i);
        i = getchar();
    }
    printf("Number of characters in file = %d\n", nc);
    return (0);
}

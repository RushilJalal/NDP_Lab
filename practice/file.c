#include <stdio.h>

int main()
{
    char *filename = "example.txt";
    char c;
    int words = 0;
    FILE *fp = fopen(filename, "r");

    while ((c = fgetc(fp)) != EOF)
    {
        if (c == ' ')
        {
            words++;
        }
    }

    printf("Number of words: %d", words + 1);
    return 0;
}

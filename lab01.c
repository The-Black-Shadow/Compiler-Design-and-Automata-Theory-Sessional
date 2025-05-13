#include <stdio.h>

int main()
{
    char line[1000];

    printf("Enter a line: ");
    fgets(line, sizeof(line), stdin);

    if ((line[0] == '/' && line[1] == '/') || (line[0] == '/' && line[1] == '*'))
    {
        printf("Comment\n");
    }
    else
    {
        printf("Not a comment\n");
    }

    return 0;
}

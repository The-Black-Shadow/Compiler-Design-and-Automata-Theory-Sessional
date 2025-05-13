#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Function to recognize strings under 'a'
bool is_a(const char *str)
{
    return (strlen(str) == 1 && str[0] == 'a');
}

// Function to recognize strings under 'a*b+'
bool is_astar_bplus(const char *str)
{
    int i = 0;
    // Match zero or more 'a's
    while (str[i] == 'a')
    {
        i++;
    }
    // Match one or more 'b's
    if (str[i] == '\0')
    {
        return false; // Must have at least one 'b'
    }
    while (str[i] == 'b')
    {
        i++;
    }
    // Check if the entire string has been consumed
    return (str[i] == '\0');
}

// Function to recognize the string 'abb'
bool is_abb(const char *str)
{
    return (strcmp(str, "abb") == 0);
}

int main()
{
    char input[100];

    printf("Enter a string: ");
    scanf("%s", input);

    if (is_a(input))
    {
        printf("\"%s\" is recognized under 'a'.\n", input);
    }
    else if (is_abb(input))
    {
        printf("\"%s\" is recognized under 'abb'.\n", input);
    }
    else if (is_astar_bplus(input))
    {
        printf("\"%s\" is recognized under 'a*b+'.\n", input);
    }
    else
    {
        printf("\"%s\" is not recognized under any of the given patterns.\n", input);
    }

    return 0;
}
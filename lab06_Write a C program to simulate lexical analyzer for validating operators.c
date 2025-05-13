#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Define all operator arrays
// const char* arithmetic_operators[] = {"+", "-", "*", "/", "//", "%", "**"};
// const char* assignment_operators[] = {"=", "+=", "-=", "*=", "/=", "//=", "%=", "**=", "&=", "|=", "^=", "<<=", ">>="};
// const char* comparison_operators[] = {"==", "!=", "<", ">", "<=", ">="};
// const char* logical_operators[] = {"and", "or", "not"};
// const char* bitwise_operators[] = {"&", "|", "^", "~", "<<", ">>"};
// const char* membership_operators[] = {"in", "not in"};
// const char* identity_operators[] = {"is", "is not"};

// Combine all operators into a single array
const char *all_operators[] = {
    "+", "-", "*", "/", "//", "%", "**",
    "=", "+=", "-=", "*=", "/=", "//=", "%=", "**=", "&=", "|=", "^=", "<<=", ">>=",
    "==", "!=", "<", ">", "<=", ">=",
    "and", "or", "not",
    "&", "|", "^", "~", "<<", ">>",
    "in", "not in",
    "is", "is not"};

int is_valid_operator(const char *token)
{
    int num_operators = sizeof(all_operators) / sizeof(all_operators[0]);
    for (int i = 0; i < num_operators; i++)
    {
        if (strcmp(token, all_operators[i]) == 0)
        {
            return 1; // valid operator
        }
    }
    return 0; // invalid operator
}

// Simple tokenizer: splits on whitespace and symbols
void lexical_analyzer(char *input)
{
    const char *delimiters = " \t\n";
    char *token = strtok(input, delimiters);

    while (token != NULL)
    {
        if (is_valid_operator(token))
        {
            printf("Valid operator: %s\n", token);
        }
        else
        {
            printf("Invalid operator: %s\n", token);
        }
        token = strtok(NULL, delimiters);
    }
}

int main()
{
    char input[256];

    printf("Enter an expression with operators: ");
    fgets(input, sizeof(input), stdin);

    // Remove trailing newline if present
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n')
        input[len - 1] = '\0';

    printf("Input Expression: %s\n", input);

    lexical_analyzer(input);

    return 0;
}

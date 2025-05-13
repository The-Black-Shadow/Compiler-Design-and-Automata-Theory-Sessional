#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

char stack[MAX];
int top = -1;
char input[MAX];
int i = 0;

void push(char symbol)
{
    stack[++top] = symbol;
}

void pop()
{
    top--;
}

char peek()
{
    return stack[top];
}

void printStack()
{
    for (int j = 0; j <= top; j++)
    {
        printf("%c", stack[j]);
    }
    printf("\t");
}

void printInput()
{
    printf("%s\n", input + i);
}

int isTerminal(char symbol)
{
    return !(symbol == 'E' || symbol == 'T' || symbol == 'F' || symbol == 'X' || symbol == 'Y');
}

// Predictive parse table function
const char *parseTable(char nonTerminal, char token)
{
    switch (nonTerminal)
    {
    case 'E':
        if (token == 'i' || token == '(')
            return "TX";
        break;
    case 'X':
        if (token == '+')
            return "+TX";
        if (token == ')' || token == '$')
            return "ε";
        break;
    case 'T':
        if (token == 'i' || token == '(')
            return "FY";
        break;
    case 'Y':
        if (token == '*')
            return "*FY";
        if (token == '+' || token == ')' || token == '$')
            return "ε";
        break;
    case 'F':
        if (token == 'i')
            return "i";
        if (token == '(')
            return "(E)";
        break;
    }
    return NULL;
}

void predictiveParser()
{
    push('$');
    push('E');

    while (peek() != '$')
    {
        printStack();
        printInput();

        char topSymbol = peek();
        char currentToken = input[i];

        if (topSymbol == currentToken)
        {
            pop();
            i++;
        }
        else if (isTerminal(topSymbol))
        {
            printf("Error: Unexpected symbol %c\n", currentToken);
            return;
        }
        else
        {
            const char *production = parseTable(topSymbol, currentToken);
            if (production == NULL)
            {
                printf("Error: No rule for %c with input %c\n", topSymbol, currentToken);
                return;
            }

            pop();
            if (strcmp(production, "ε") != 0)
            {
                int len = strlen(production);
                for (int j = len - 1; j >= 0; j--)
                {
                    push(production[j]);
                }
            }
        }
    }

    if (input[i] == '$')
    {
        printf("Parsing successful.\n");
    }
    else
    {
        printf("Error: Input not fully consumed.\n");
    }
}

int main()
{
    printf("Enter the input string (use 'i' for id), end with $: ");
    scanf("%s", input);

    printf("\nStack\tInput\n");
    printf("------------------\n");

    predictiveParser();

    return 0;
}
// Example input: (i+i)*i$
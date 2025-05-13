#include <stdio.h>
#include <string.h>

char input[100];
int i = 0;

int match(char expected)
{
    if (input[i] == expected)
    {
        i++;
        return 1;
    }
    return 0;
}

int E(); // Forward declaration

int F()
{
    if (match('('))
    {
        if (E())
        {
            if (match(')'))
            {
                return 1;
            }
            else
            {
                return 0; // Missing closing parenthesis
            }
        }
        else
        {
            return 0; // Invalid expression inside parentheses
        }
    }
    else if (match('i'))
    {
        return 1; // Identifier matched
    }
    else
    {
        return 0; // Invalid factor
    }
}

int Tx()
{
    if (match('*'))
    {
        if (F())
        {
            if (Tx())
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    return 1; // ε-production
}

int T()
{
    if (F())
    {
        if (Tx())
        {
            return 1;
        }
    }
    return 0;
}

int Ex()
{
    if (match('+'))
    {
        if (T())
        {
            if (Ex())
            {
                return 1;
            }
        }
        return 0;
    }
    return 1; // ε-production
}

int E()
{
    if (T())
    {
        if (Ex())
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    printf("Recursive Descent Parser for the grammar:\n");
    printf("E -> T Ex\nEx -> + T Ex | ε\nT -> F Tx\nTx -> * F Tx | ε\nF -> (E) | i\n\n");

    printf("Enter the input string (end with $): ");
    scanf("%s", input);

    i = 0;
    if (E() && input[i] == '$')
    {
        printf("String is accepted.\n");
    }
    else
    {
        printf("String is not accepted. Error at position %d.\n", i);
    }

    return 0;
}
// test cases: (i*i)+i$
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

// List of C and C++ reserved words
const char *reserved_c_cpp[] = {
    // C keywords
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
    "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return",
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned",
    "void", "volatile", "while",

    // C++ specific
    "bool", "class", "const_cast", "delete", "dynamic_cast", "explicit", "export", "false",
    "friend", "inline", "mutable", "namespace", "new", "operator", "private", "protected",
    "public", "reinterpret_cast", "static_cast", "template", "this", "throw", "true", "try",
    "typeid", "typename", "using", "virtual", "wchar_t"};
const int reserved_c_cpp_count = sizeof(reserved_c_cpp) / sizeof(reserved_c_cpp[0]);

// List of Python reserved words
const char *reserved_python[] = {
    "if", "else", "while", "for", "int", "float", "return", "def", "class", "import", "from",
    "abs", "all", "any", "bin", "bool", "bytearray", "bytes", "callable", "chr", "classmethod",
    "compile", "complex", "delattr", "dict", "dir", "divmod", "enumerate", "eval", "exec",
    "filter", "format", "frozenset", "getattr", "globals", "hasattr", "hash", "help",
    "hex", "id", "input", "isinstance", "issubclass", "iter", "len", "list", "locals",
    "map", "max", "memoryview", "min", "next", "object", "oct", "open", "ord", "pow", "print",
    "property", "range", "repr", "reversed", "round", "set", "setattr", "slice", "sorted",
    "staticmethod", "str", "sum", "super", "tuple", "type", "vars", "zip"};
const int reserved_python_count = sizeof(reserved_python) / sizeof(reserved_python[0]);

// Function to check if a string is in a reserved keyword list
bool is_in_list(const char *str, const char **list, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(str, list[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// Check if a string is a valid identifier
bool is_valid_identifier(const char *str)
{
    // Check if the string is empty
    if (!str || !str[0])
        return false;

    // Check if the first character is a letter or underscore
    if (!(isalpha(str[0]) || str[0] == '_'))
    {
        return false;
    }

    // Check the rest of the characters alphanumeric or underscore
    for (int i = 1; str[i] != '\0'; i++)
    {
        if (!(isalnum(str[i]) || str[i] == '_'))
        {
            return false;
        }
    }

    // Check against reserved C/C++ and Python keywords
    if (is_in_list(str, reserved_c_cpp, reserved_c_cpp_count) ||
        is_in_list(str, reserved_python, reserved_python_count))
    {
        return false;
    }

    return true;
}

int main()
{
    char input[100];

    printf("Enter an identifier: ");
    scanf("%s", input);

    if (is_valid_identifier(input))
    {
        printf("\"%s\" is a valid identifier.\n", input);
    }
    else
    {
        printf("\"%s\" is NOT a valid identifier.\n", input);
    }

    return 0;
}

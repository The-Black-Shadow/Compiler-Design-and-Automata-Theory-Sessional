#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_PRODUCTIONS 20
#define MAX_SYMBOLS 10 // Max length of RHS of a production
#define MAX_SET_SIZE 20
#define MAX_TERMINALS 20
#define MAX_NON_TERMINALS 10

// --- Data Structures ---
char productions[MAX_PRODUCTIONS][MAX_SYMBOLS]; // Stores productions, e.g., "E=TR"
int num_productions = 0;

char non_terminals[MAX_NON_TERMINALS];
int num_non_terminals = 0;

char terminals[MAX_TERMINALS];
int num_terminals = 0;

// FIRST sets: first_sets[non_terminal_index][symbol_index]
char first_sets[MAX_NON_TERMINALS][MAX_SET_SIZE];
int first_set_sizes[MAX_NON_TERMINALS];

// FOLLOW sets: follow_sets[non_terminal_index][symbol_index]
char follow_sets[MAX_NON_TERMINALS][MAX_SET_SIZE];
int follow_set_sizes[MAX_NON_TERMINALS];

// LL(1) Parsing Table: table[non_terminal_index][terminal_index] stores production index or -1 for error
int parsing_table[MAX_NON_TERMINALS][MAX_TERMINALS];
char table_productions[MAX_NON_TERMINALS][MAX_TERMINALS][MAX_SYMBOLS]; // To store actual production string

char start_symbol;

// --- Helper Functions ---

// Function to add a character to a set if not already present
void add_to_set(char *set, int *set_size, char symbol) {
    if (symbol == '\0') return;
    for (int i = 0; i < *set_size; i++) {
        if (set[i] == symbol) {
            return;
        }
    }
    set[(*set_size)++] = symbol;
}

// Get index of a non-terminal
int get_non_terminal_index(char nt) {
    for (int i = 0; i < num_non_terminals; i++) {
        if (non_terminals[i] == nt) {
            return i;
        }
    }
    return -1; // Should not happen if grammar is processed correctly
}

// Get index of a terminal
int get_terminal_index(char t) {
    if (t == '$') return num_terminals -1; // Assuming '$' is the last terminal
    for (int i = 0; i < num_terminals; i++) {
        if (terminals[i] == t) {
            return i;
        }
    }
    return -1; // Should not happen
}

// --- FIRST Set Calculation ---
void find_first_recursive(char symbol, int nt_index_orig, int prod_idx_rhs, int current_prod_idx);

// Calculates FIRST set for a given symbol (non-terminal or sequence)
// For a single non-terminal X, call with first_of_X(X_char_itself, first_sets[index_of_X], &first_set_sizes[index_of_X])
// For a string alpha (part of RHS), iterate through alpha
void calculate_first_for_string(const char* alpha, char* resultSet, int* resultSize) {
    int k = 0;
    *resultSize = 0; // Reset result set

    while (alpha[k] != '\0') {
        char current_symbol = alpha[k];
        int has_epsilon = 0;

        if (isupper(current_symbol)) { // Non-terminal
            int nt_idx = get_non_terminal_index(current_symbol);
            for (int i = 0; i < first_set_sizes[nt_idx]; i++) {
                if (first_sets[nt_idx][i] == '#') {
                    has_epsilon = 1;
                } else {
                    add_to_set(resultSet, resultSize, first_sets[nt_idx][i]);
                }
            }
        } else { // Terminal
            add_to_set(resultSet, resultSize, current_symbol);
            break; // Terminal found, stop
        }

        if (!has_epsilon) {
            break; // If current symbol's FIRST doesn't have epsilon, stop
        }
        k++; // Move to next symbol in alpha
    }
    // If all symbols in alpha derived epsilon, add epsilon to FIRST(alpha)
    if (alpha[k] == '\0' && k > 0) { // Check if loop finished and alpha was not empty
        int all_epsilon = 1;
        for(int i=0; alpha[i] != '\0'; ++i){
            if(isupper(alpha[i])){
                int nt_idx_temp = get_non_terminal_index(alpha[i]);
                int current_has_epsilon = 0;
                for(int j=0; j<first_set_sizes[nt_idx_temp]; ++j){
                    if(first_sets[nt_idx_temp][j] == '#'){
                        current_has_epsilon = 1;
                        break;
                    }
                }
                if(!current_has_epsilon){
                    all_epsilon = 0;
                    break;
                }
            } else { // Terminal
                all_epsilon = 0;
                break;
            }
        }
        if(all_epsilon) add_to_set(resultSet, resultSize, '#');
    } else if (alpha[0] == '#' && alpha[1] == '\0') { // Special case for production like A -> #
         add_to_set(resultSet, resultSize, '#');
    }
}


void compute_first_sets() {
    int changed;
    // Initialize FIRST sets
    for (int i = 0; i < num_non_terminals; i++) {
        first_set_sizes[i] = 0;
    }

    // Iteratively compute FIRST sets
    do {
        changed = 0;
        for (int i = 0; i < num_productions; i++) {
            char nt = productions[i][0];
            int nt_idx = get_non_terminal_index(nt);
            char *rhs = &productions[i][2]; // Skip "X="

            char first_of_rhs[MAX_SET_SIZE];
            int first_of_rhs_size = 0;

            if (rhs[0] == '#') { // Production is X -> #
                int old_size = first_set_sizes[nt_idx];
                add_to_set(first_sets[nt_idx], &first_set_sizes[nt_idx], '#');
                if (first_set_sizes[nt_idx] > old_size) changed = 1;
                continue;
            }

            // For X -> Y1Y2...Yk
            int k = 0;
            int continue_to_next_symbol = 1;
            while (rhs[k] != '\0' && continue_to_next_symbol) {
                char current_symbol = rhs[k];
                continue_to_next_symbol = 0; // Assume we won't continue unless epsilon is found

                if (islower(current_symbol) || !isalpha(current_symbol)) { // Terminal
                    int old_size = first_set_sizes[nt_idx];
                    add_to_set(first_sets[nt_idx], &first_set_sizes[nt_idx], current_symbol);
                    if (first_set_sizes[nt_idx] > old_size) changed = 1;
                    break; // Stop with this terminal
                } else if (isupper(current_symbol)) { // Non-terminal
                    int symbol_nt_idx = get_non_terminal_index(current_symbol);
                    for (int j = 0; j < first_set_sizes[symbol_nt_idx]; j++) {
                        if (first_sets[symbol_nt_idx][j] == '#') {
                            continue_to_next_symbol = 1; // Epsilon found, may need to check next symbol
                        } else {
                            int old_size = first_set_sizes[nt_idx];
                            add_to_set(first_sets[nt_idx], &first_set_sizes[nt_idx], first_sets[symbol_nt_idx][j]);
                            if (first_set_sizes[nt_idx] > old_size) changed = 1;
                        }
                    }
                    if (!continue_to_next_symbol) break; // If no epsilon in FIRST(current_symbol), stop
                }
                k++;
            }
            // If all symbols in RHS derive epsilon, add epsilon to FIRST(nt)
            if (rhs[k] == '\0' && continue_to_next_symbol) {
                 int old_size = first_set_sizes[nt_idx];
                 add_to_set(first_sets[nt_idx], &first_set_sizes[nt_idx], '#');
                 if (first_set_sizes[nt_idx] > old_size) changed = 1;
            }
        }
    } while (changed);
}


// --- FOLLOW Set Calculation ---
void compute_follow_sets() {
    int changed;
    // Initialize FOLLOW sets
    for (int i = 0; i < num_non_terminals; i++) {
        follow_set_sizes[i] = 0;
    }

    // Rule 1: Place $ in FOLLOW(S) where S is the start symbol
    int start_idx = get_non_terminal_index(start_symbol);
    if (start_idx != -1) {
        add_to_set(follow_sets[start_idx], &follow_set_sizes[start_idx], '$');
    }

    do {
        changed = 0;
        for (int i = 0; i < num_productions; i++) { // For each production A -> alpha
            char A = productions[i][0];
            int A_idx = get_non_terminal_index(A);
            char *alpha = &productions[i][2];

            for (int j = 0; alpha[j] != '\0'; j++) { // Iterate through RHS symbols
                char B = alpha[j];
                if (isupper(B)) { // If B is a non-terminal
                    int B_idx = get_non_terminal_index(B);
                    char *beta = &alpha[j + 1]; // Symbols after B

                    // Rule 2: A -> alpha B beta
                    // Add FIRST(beta) - {#} to FOLLOW(B)
                    char first_of_beta[MAX_SET_SIZE];
                    int first_of_beta_size = 0;
                    if (beta[0] != '\0') { // If beta is not empty
                         calculate_first_for_string(beta, first_of_beta, &first_of_beta_size);
                    }


                    int beta_has_epsilon = 0;
                    for (int k = 0; k < first_of_beta_size; k++) {
                        if (first_of_beta[k] == '#') {
                            beta_has_epsilon = 1;
                        } else {
                            int old_size = follow_set_sizes[B_idx];
                            add_to_set(follow_sets[B_idx], &follow_set_sizes[B_idx], first_of_beta[k]);
                            if (follow_set_sizes[B_idx] > old_size) changed = 1;
                        }
                    }

                    // Rule 3: A -> alpha B (beta is empty) OR A -> alpha B beta where FIRST(beta) contains #
                    // Add FOLLOW(A) to FOLLOW(B)
                    if (beta[0] == '\0' || beta_has_epsilon) {
                        for (int k = 0; k < follow_set_sizes[A_idx]; k++) {
                            int old_size = follow_set_sizes[B_idx];
                            add_to_set(follow_sets[B_idx], &follow_set_sizes[B_idx], follow_sets[A_idx][k]);
                             if (follow_set_sizes[B_idx] > old_size) changed = 1;
                        }
                    }
                }
            }
        }
    } while (changed);
}


// --- LL(1) Parsing Table Construction ---
void construct_parsing_table() {
    // Initialize table with -1 (error)
    for (int i = 0; i < num_non_terminals; i++) {
        for (int j = 0; j < num_terminals; j++) {
            parsing_table[i][j] = -1; // -1 indicates error or empty slot
            strcpy(table_productions[i][j], "error");
        }
    }

    for (int i = 0; i < num_productions; i++) { // For each production A -> alpha
        char A = productions[i][0];
        char *alpha = &productions[i][2];
        int A_idx = get_non_terminal_index(A);

        char first_of_alpha[MAX_SET_SIZE];
        int first_of_alpha_size = 0;
        calculate_first_for_string(alpha, first_of_alpha, &first_of_alpha_size);


        int alpha_has_epsilon = 0;
        // Rule 1: For each terminal 'a' in FIRST(alpha), add A -> alpha to Table[A, a]
        for (int k = 0; k < first_of_alpha_size; k++) {
            char terminal_a = first_of_alpha[k];
            if (terminal_a == '#') {
                alpha_has_epsilon = 1;
            } else {
                int term_idx = get_terminal_index(terminal_a);
                if (term_idx != -1) {
                    if (parsing_table[A_idx][term_idx] != -1) {
                        fprintf(stderr, "Error: Grammar is not LL(1). Conflict at Table[%c, %c] for production %s and %s\n",
                                A, terminal_a, table_productions[A_idx][term_idx], productions[i]);
                        exit(1);
                    }
                    parsing_table[A_idx][term_idx] = i; // Store production index
                    strcpy(table_productions[A_idx][term_idx], productions[i]);
                }
            }
        }

        // Rule 2: If # is in FIRST(alpha), then for each terminal 'b' in FOLLOW(A) (including $),
        // add A -> alpha to Table[A, b]
        if (alpha_has_epsilon) {
            for (int k = 0; k < follow_set_sizes[A_idx]; k++) {
                char terminal_b = follow_sets[A_idx][k]; // terminal_b can be '$'
                int term_idx = get_terminal_index(terminal_b);

                if (term_idx != -1) {
                     if (parsing_table[A_idx][term_idx] != -1 && strcmp(table_productions[A_idx][term_idx], productions[i]) != 0 ) {
                        // Check if the existing production is different from the current one
                        // This can happen if A-># and also some other A->gamma exists where # is in FIRST(gamma)
                        // and current terminal_b is also in FIRST(gamma).
                        // More robust conflict detection needed if multiple A-># like productions.
                        // For simplicity, if a different prod is already there, it's a conflict.
                        fprintf(stderr, "Error: Grammar is not LL(1). Conflict at Table[%c, %c] for production %s and %s\n",
                                A, terminal_b, table_productions[A_idx][term_idx], productions[i]);
                        exit(1);
                    }
                    parsing_table[A_idx][term_idx] = i;
                    strcpy(table_productions[A_idx][term_idx], productions[i]);
                }
            }
        }
    }
}

// --- LL(1) Parsing Algorithm ---
void parse_input_string(const char *input) {
    char stack[100];
    int top = -1;
    char current_input[100];
    strcpy(current_input, input);
    strcat(current_input, "$"); // Append end marker

    int ip = 0; // Input pointer

    // Initial stack: $S
    stack[++top] = '$';
    stack[++top] = start_symbol;

    printf("\n--- Parsing Steps ---\n");
    printf("Stack\t\tInput\t\tAction\n");
    printf("-----\t\t-----\t\t------\n");

    while (top != -1) {
        // Print current stack
        for (int i = top; i >= 0; i--) printf("%c", stack[i]);
        printf("\t\t%s\t\t", &current_input[ip]);

        char X = stack[top]; // Top of stack
        char a = current_input[ip]; // Current input symbol

        if (X == '$' && a == '$') {
            printf("Accept\n");
            return;
        }

        if (X == a) { // Match
            printf("Match %c\n", a);
            top--; // Pop
            ip++;  // Advance input
        } else if (islower(X) || !isalpha(X) && X != '#') { // X is terminal but not matching 'a'
             printf("Error: Mismatch (Terminal on stack: %c, Input: %c)\n", X, a);
             return;
        } else if (isupper(X)) { // X is a non-terminal
            int nt_idx = get_non_terminal_index(X);
            int term_idx = get_terminal_index(a);

            if (nt_idx == -1 || term_idx == -1) {
                printf("Error: Invalid non-terminal/terminal in parsing table lookup (%c, %c)\n", X, a);
                return;
            }

            int prod_idx_to_apply = parsing_table[nt_idx][term_idx];

            if (prod_idx_to_apply == -1) {
                printf("Error: No rule in parsing table for M[%c, %c]\n", X, a);
                return;
            }

            // Apply production X -> Y1Y2...Yk
            char *rhs = &productions[prod_idx_to_apply][2];
            printf("Apply %s\n", productions[prod_idx_to_apply]);
            top--; // Pop X

            if (rhs[0] != '#') { // If not epsilon production
                int len = strlen(rhs);
                for (int k = len - 1; k >= 0; k--) { // Push Yk...Y1 (Y1 on top)
                    stack[++top] = rhs[k];
                }
            }
        } else { // Should not happen if stack only contains NT, T, $
            printf("Error: Invalid symbol on stack %c\n", X);
            return;
        }
    }
    printf("Error: Input not fully consumed or stack not empty correctly.\n");
}


// --- Main Function ---
int main() {
    printf("Enter the number of productions: ");
    scanf("%d", &num_productions);
    getchar(); // Consume newline

    printf("Enter the productions (e.g., E=TR, R=# for epsilon):\n");
    for (int i = 0; i < num_productions; i++) {
        printf("Production %d: ", i + 1);
        fgets(productions[i], MAX_SYMBOLS, stdin);
        productions[i][strcspn(productions[i], "\n")] = 0; // Remove newline

        // Populate non_terminals
        char nt = productions[i][0];
        int found_nt = 0;
        for (int k = 0; k < num_non_terminals; k++) {
            if (non_terminals[k] == nt) {
                found_nt = 1;
                break;
            }
        }
        if (!found_nt && num_non_terminals < MAX_NON_TERMINALS) {
            non_terminals[num_non_terminals++] = nt;
        }

        // Populate terminals
        for (int j = 2; productions[i][j] != '\0'; j++) { // Start from RHS
            char sym = productions[i][j];
            if (!isupper(sym) && sym != '#') { // Terminal
                int found_t = 0;
                for (int k = 0; k < num_terminals; k++) {
                    if (terminals[k] == sym) {
                        found_t = 1;
                        break;
                    }
                }
                if (!found_t && num_terminals < MAX_TERMINALS -1) { // -1 for '$'
                    terminals[num_terminals++] = sym;
                }
            }
        }
    }
    // Add $ as a terminal
    terminals[num_terminals++] = '$';


    if (num_productions > 0) {
        start_symbol = productions[0][0];
    } else {
        printf("No productions entered.\n");
        return 1;
    }

    printf("\n--- Productions ---\n");
    for (int i = 0; i < num_productions; i++) {
        printf("%s\n", productions[i]);
    }
    printf("Start Symbol: %c\n", start_symbol);

    printf("\nNon-Terminals: ");
    for(int i=0; i<num_non_terminals; ++i) printf("%c ", non_terminals[i]);
    printf("\nTerminals: ");
    for(int i=0; i<num_terminals; ++i) printf("%c ", terminals[i]);
    printf("\n");

    // --- Calculate FIRST Sets ---
    compute_first_sets();
    printf("\n--- FIRST Sets ---\n");
    for (int i = 0; i < num_non_terminals; i++) {
        printf("FIRST(%c) = { ", non_terminals[i]);
        for (int j = 0; j < first_set_sizes[i]; j++) {
            printf("%c ", first_sets[i][j]);
        }
        printf("}\n");
    }

    // --- Calculate FOLLOW Sets ---
    compute_follow_sets();
    printf("\n--- FOLLOW Sets ---\n");
    for (int i = 0; i < num_non_terminals; i++) {
        printf("FOLLOW(%c) = { ", non_terminals[i]);
        for (int j = 0; j < follow_set_sizes[i]; j++) {
            printf("%c ", follow_sets[i][j]);
        }
        printf("}\n");
    }

    // --- Construct Parsing Table ---
    construct_parsing_table();
    printf("\n--- LL(1) Parsing Table ---\n");
    printf("\t");
    for (int j = 0; j < num_terminals; j++) {
        printf("%c\t", terminals[j]);
    }
    printf("\n");
    for (int i = 0; i < num_non_terminals; i++) {
        printf("%c\t", non_terminals[i]);
        for (int j = 0; j < num_terminals; j++) {
            if (parsing_table[i][j] != -1) {
                printf("%s\t", table_productions[i][j]);
            } else {
                printf("error\t");
            }
        }
        printf("\n");
    }

    // --- Parse Input String ---
    char input_string[100];
    printf("\nEnter the input string to parse (without $): ");
    scanf("%s", input_string);

    parse_input_string(input_string);

    return 0;
}

/*
Example Grammar (No left recursion, left-factored):
E=TR
R=+TR
R=#
T=FY
Y=*FY
Y=#
F=(E)
F=i

Input for the program:
Number of productions: 8
Productions:
E=TR
R=+TR
R=#
T=FY
Y=*FY
Y=#
F=(E)
F=i

Example input string to parse: i+i*i  or (i+i)

Expected FIRST sets:
FIRST(E) = { (, i }
FIRST(R) = { +, # }
FIRST(T) = { (, i }
FIRST(Y) = { *, # }
FIRST(F) = { (, i }

Expected FOLLOW sets:
FOLLOW(E) = { $, ) }
FOLLOW(R) = { $, ) }
FOLLOW(T) = { +, $, ) }
FOLLOW(Y) = { +, $, ) }
FOLLOW(F) = { *, +, $, ) }
*/
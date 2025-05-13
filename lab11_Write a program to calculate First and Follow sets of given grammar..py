#Write a program to calculate First and Follow sets of given grammar.
#G: {E-> TE’, E’ -> +TE’ | 0, T-> FT’, T’-> *FT’|0, F-> (E) | id}.

from collections import defaultdict

# Grammar definition
grammar = {
    'E': [['T', "E'"]],
    "E'": [['+', 'T', "E'"], ['ε']],
    'T': [['F', "T'"]],
    "T'": [['*', 'F', "T'"], ['ε']],
    'F': [['(', 'E', ')'], ['id']]
}

non_terminals = set(grammar.keys())
terminals = set()

# Extract terminals
for rules in grammar.values():
    for rule in rules:
        for symbol in rule:
            if symbol not in grammar and symbol != 'ε':
                terminals.add(symbol)

FIRST = defaultdict(set)
FOLLOW = defaultdict(set)

# Function to compute FIRST of a symbol
def first(symbol):
    if symbol in FIRST and FIRST[symbol]:
        return FIRST[symbol]
    if symbol not in non_terminals:
        return {symbol}

    result = set()
    for rule in grammar[symbol]:
        for sym in rule:
            sym_first = first(sym)
            result.update(sym_first - {'ε'})
            if 'ε' not in sym_first:
                break
        else:
            result.add('ε')
    FIRST[symbol] = result
    return result

# Function to compute FOLLOW sets
def compute_follow():
    FOLLOW['E'].add('$')  # Start symbol

    changed = True
    while changed:
        changed = False
        for lhs in grammar:
            for rule in grammar[lhs]:
                for i in range(len(rule)):
                    B = rule[i]
                    if B in non_terminals:
                        follow_before = FOLLOW[B].copy()

                        # Consider the rest of the symbols after B
                        rest = rule[i+1:]
                        if rest:
                            first_rest = set()
                            for sym in rest:
                                sym_first = first(sym)
                                first_rest.update(sym_first - {'ε'})
                                if 'ε' not in sym_first:
                                    break
                            else:
                                first_rest.add('ε')

                            FOLLOW[B].update(first_rest - {'ε'})

                            if 'ε' in first_rest:
                                FOLLOW[B].update(FOLLOW[lhs])
                        else:
                            FOLLOW[B].update(FOLLOW[lhs])

                        if follow_before != FOLLOW[B]:
                            changed = True

# Compute FIRST sets
for non_terminal in grammar:
    first(non_terminal)

# Compute FOLLOW sets
compute_follow()

# Display the results
print("FIRST sets:")
for nt in grammar:
    print(f"FIRST({nt}) = {FIRST[nt]}")

print("\nFOLLOW sets:")
for nt in grammar:
    print(f"FOLLOW({nt}) = {FOLLOW[nt]}")

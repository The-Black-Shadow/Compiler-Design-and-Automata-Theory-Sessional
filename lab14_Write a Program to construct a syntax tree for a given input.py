import ast
import astpretty
from graphviz import Digraph
import uuid


def construct_syntax_tree(expr):
    try:
        # Textual AST pretty print
        tree = ast.parse(expr, mode='eval')
        print("Syntax Tree for:", expr)
        astpretty.pprint(tree)

        # Visual tree generation
        dot = Digraph(comment="AST")
        build_graph(dot, tree)
        dot.render("syntax_tree", format="png", cleanup=True)
        print("Visual syntax tree saved as 'syntax_tree.png'.")

    except SyntaxError as e:
        print("Syntax Error:", e)


def build_graph(dot, node, parent=None):
    node_id = str(uuid.uuid4())
    label = type(node).__name__

    # Add node to graph
    dot.node(node_id, label)

    # Connect to parent
    if parent:
        dot.edge(parent, node_id)

    # Traverse child nodes
    for child_name, child in ast.iter_fields(node):
        if isinstance(child, ast.AST):
            build_graph(dot, child, node_id)
        elif isinstance(child, list):
            for item in child:
                if isinstance(item, ast.AST):
                    build_graph(dot, item, node_id)
        else:
            # Terminal values (e.g., numbers, identifiers)
            value_id = str(uuid.uuid4())
            dot.node(value_id, f"{child_name}={repr(child)}", shape="box", style="filled", fillcolor="lightgrey")
            dot.edge(node_id, value_id)


if __name__ == "__main__":
    expression = input("Enter an arithmetic expression: ")
    construct_syntax_tree(expression)

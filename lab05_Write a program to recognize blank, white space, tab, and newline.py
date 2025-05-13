def recognize_whitespace_in_python(file_path):
    # Initialize counts
    blank_count = 0
    # whitespace_count = 0
    tab_count = 0
    newline_count = 0

    try:
        # Read the Python code from the file
        with open(file_path, 'r', encoding='utf-8') as file:
            python_code = file.read()

        # Iterate through each character in the Python code
        for char in python_code:
            if char == ' ':
                blank_count += 1
            # elif char.isspace():
            #     whitespace_count += 1
            elif char == '\t':
                tab_count += 1
            elif char == '\n':
                newline_count += 1

    except FileNotFoundError:
        print(f"File '{file_path}' not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

    return blank_count, tab_count, newline_count

if __name__ == "__main__":
    # Hardcoded file path for testing
    hardcoded_file_path = "example.py"

    # Call the function to recognize whitespace in the Python code
    blank, tab, newline = recognize_whitespace_in_python(hardcoded_file_path)

    # Display the results
    print(f"Occurrences of White spaces: {blank}")
    # print(f"Occurrences of white spaces, tabs, and newlines: {whitespace}")
    print(f"Occurrences of tabs: {tab}")
    print(f"Occurrences of newlines: {newline}")
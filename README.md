# myshell

Define a function to parse user input into a list of command and argument strings. This function should handle whitespace and quoted arguments correctly.

If mysh is called with an argument, open the specified file and read its contents into a list of command strings.

If mysh is not called with an argument, enter an input loop that prompts the user for input and calls the command parsing function on each line of input.

For each command string, execute it by calling the appropriate system function (e.g., os.system()) with the command and argument strings as arguments.

If a command is "exit", terminate mysh and return control to the calling program.
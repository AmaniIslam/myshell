# myshell

Logic and Algorithm 

main method: 

First, program will figure what mode the user has chosen. 

If program is in batch mode the program will open the file using the open() function and storing the file descriptor. Batch mode if there is 2 arguments including /mysh. 

If the program has no arguments beside /mysh then it is interactive mode. Where the file descriptor will be assigned as STDIN_Fileno to read from standard input. 

Anything else will return an error...

Parsing algorithm - using one input while loop 
Using the read() function will read and store the bytes that have been read in ssize_t nBytes. It will also take the buffer array which has been allocated a fix size 1024 bytes 


3.3 issue where it only works when the thing is not ran as the first command it lists everything in the thing

TESTCASES
pwd
~/
pwd

The ">" symbol is used to redirect the output of a command to a file. For example, ls > file.txt will run the ls command and save its output to a file called "file.txt".
output

The "<" symbol is used to redirect input from a file to a command. For example, sort < file.txt will sort the contents of the "file.txt" file.
input


Define a function to parse user input into a list of command and argument strings. This function should handle whitespace and quoted arguments correctly.

If mysh is called with an argument, open the specified file and read its contents into a list of command strings.

If mysh is not called with an argument, enter an input loop that prompts the user for input and calls the command parsing function on each line of input.

For each command string, execute it by calling the appropriate system function (e.g., os.system()) with the command and argument strings as arguments.

If a command is "exit", terminate mysh and return control to the calling program.









Parse the command line to identify any file redirections. For example, if the command is ls > output.txt, the redirection operator > indicates that the output of the ls command should be written to the file output.txt.

Use the open() system call to open the specified file. In the example above, the shell would call open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666) to open output.txt for writing. This would create the file if it doesn't already exist, truncate it to zero length if it does exist, and set its permissions to 0666.

Use dup2() to redirect the standard output of the child process to the file descriptor returned by open(). In the example above, the shell would call dup2(file_descriptor, STDOUT_FILENO) to redirect the standard output of the child process to the output.txt file.

Execute the command in the child process using execvp() or a similar function.

The child process will now write its output to the file specified in the redirection operator.

The same approach can be used to redirect the standard input of a child process using dup2(), for example by using the < operator to specify an input file.
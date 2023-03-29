# myshell

Logic and Algorithm 

main method:
First, program will figure what mode the user has chosen. 

If program is in batch mode the program will open the file using the open() function and storing the file descriptor. Batch mode if there is 2 arguments including /mysh. 

If the program has no arguments beside /mysh then it is interactive mode. Where the file descriptor will be assigned as STDIN_FILENO to read from standard input. 

Anything else will return an error...

Parsing algorithm - using one input while loop 
Using the read() function will read and store the bytes that have been read in ssize_t nBytes. It will also take the buffer array which has been allocated a fix size 1024 bytes as well as the BUFF_SIZE Macro which is 1024 bytes.The loop will continuously read until there are bytes to read and the bytesread is greater than 0. Moving into the inside of the while loop, a foor loop will iterate through each byte that was read in the read() function. As it is iterating through the bytes, if the buffer area reaches the end of the line('\n') the line will set a null termination character which indicates the end of the string. If buffer[i] != '\n' then buffer[i] will populate the line array. Finally for command execution, based on the current mode it will use pass the line array and flag that indicates its mode to the execCommand(). This function will take the lines and tokenize each word in the line in preparation to execute and direct to specific functions. 

execCommand():
First, we will start off by creating a char array - char*arr[BUFF_SIZE] setting its value to 1024 bytes. And like mentioned before we will create a token pointer that is used to break the string into smaller chunks using the delimiter a character space, tab, or newline character-" \t\n". Then in a while loop I want to traverse through each command at a time checking for various subfunctions like fileredirections, pipes, and wildcards all while token != NULL. The iterator in this situation will be token = strtok(NULL, " \t\n"); allowing us to get to the following token. aCount will also be keeping track of the number of tokens in the string and will be populating arr, which is the array we have created beforehand. As we are traversing through the string for a variety of characteristics symbolizing different functions: 

Checking if 
1. if (token[0] == '>') = File redirection
2. if (token[0] == '<') = File redirection 
3. if (strchr(token, '*') != NULL) - Wildcard- Checking to see if there are any astericks using -strchr which returns a non-null value if an asterick is present in each token. If an asterick is a present in any of the strings it will hold the position of that using a seperate variable. And also create a wCard flag for its execution after populating arr. 
4. if (token[0] == '|') = Pipes - 

Once arr is populated and once we exit the while loop we will set the last index arr[aCount] = NULL for execvp function.  

Moving on to wildcards...using the flag created earlier. We first duplicated arr and populated all its values into copyArr for later use. Using glob.h and glob. Start by creating a global variable to hold the result of glob() - glob_t globbuf. Next use glob function to retrieve a return value of val when you pass the arr position where wildcard occurs, and some error checks, and &globbuf. Val will return a 0 if glob() has ran successfully. Using a for loop we will traverse through each match globbuf.gl_pathc will ensure that the while loop only executes for the number of matches found. Once they are found we will feed those values into arr[pos]. 
If there is no match we will simply exit the program. Also globfree(&globbuf) will be needed to 


EXTENSIONS:
3.2
3.3 Directory WildCards



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
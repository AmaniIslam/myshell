# myshell
#Amani Islam - ai326
#Madhur Nutulaputi - mn712
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

Moving on to wildcards...using the flag created earlier. We first duplicated arr and populated all its values into copyArr for later use. Using glob.h and glob. Start by creating a global variable to hold the result of glob() - glob_t globbuf. Next use glob function to retrieve a return value of val when you pass the arr position where wildcard occurs, and some error checks, and &globbuf. Val will return a 0 if glob() has ran successfully. Using a for loop we will traverse through each match globbuf.gl_pathc will ensure that the while loop only executes for the number of matches found. Once they are found we will feed those values into arr[pos]. If there is no match we will simply exit the program. Also globfree(&globbuf) will be needed to deallocate memory.
AMANI WILL EXPLAIN EVERYTHING AFTER this point.

pwd_cd() - handles built in functions pwd cd - 

pwd : will check the first index to see if its equal to the string "pwd". Considering it does we have an error handle to make sure that the pwd has no additional arguments by checking to make sure arr[1]=NULL. Next, we will create an array and use getcwd() to retrieve current working directory and store it in the cwd character array. Finally will print the path. 

cd : 
if one argument passed we will use chdir[arr[1] to change the directory to the intended one. 
3.2 Extension part - Very similar to the extension refer to that explanation. Only difference is that we will use strcpy to copy the homeDirecter to homepath. And then we will change directory to homepath.  


introTag() - will be called only in interactive to print mysh> and will print !mysh> for an error statement. 

<<<<<<< HEAD
re_pipe() - Combination of redirection and pipe. This function takes in several arguments first and second arrays, output char pointer, char input pointer, and char subOutput pointer. First we will initialize the file descripters for pipes in an array of two indicies. First, we'll check to see if there is a second command to execute and then create a pipe between two commands. Next we will fork the first child process and if pid1==0 we will enter into the first child process. We redirect standard output to the output file if it has been described. Open the file for 0_WRONLY.    
=======
re_pipe() - Combination of redirection and pipe. This function takes in several arguments: the arrays of the first and second commands and possible output, input, and subcommand output redirect files. First we will initialize the file descripters for pipes or redirect in an array of two indicies.
    - For Redirect: First, we create a child process. We will open the file corresponding to whether it is output or input. We will also duplicate the corresponding file descriptor to STDOUT or STDIN. Finally, we execute the command using execvp.
    - For Pipe: First, we'll check to see if there is a second command to execute and then create a pipe between two commands. We will execute the necessary processes for the first command, then create another child process for the subcommand. We will repeat steps for output if necessary and then execute the subcommand.
>>>>>>> a811be87032c0952e61581daba100d9272a03409

EXTENSIONS:
3.2 - Home Directory - Part 1: -First will check for the occurance of ~/ as a token. Create a homepath arr- homepath[BUFF_SIZE]. Then create a homeDirectory pointer that retrieves the value of HOME Environmental Variabl using getenvfunction. Then snprintf will concatenate the homedirectory path with the second arg in arr*. Finally chdir will change to the newly formed path.
Part 2 : cd  with no arguments refer above. 

3.3 Directory WildCards
<<<<<<< HEAD
Our implementation for wildcards cohesively and collaterly works with directory wildcards as well. We will check for the occurance of */*.c and will set a flag and will proceed into wildcard algorithm. 
=======
Our implementation for wildcards cohesively and collaterly works with directory wildcards as well. We will check for the occurance of */*.c and will set a flag and will proceed into wildcard algorithm.

ERROR: Every possible command will execute perfectly if it is the first command after running mysh. Once a command involving pipes is executed, the commands after it do not execute properly due to a complication involving parent and child processes.

Testing: The files for testing batchmode are error.sh, redirect.sh, wildcard.sh, pipes.sh
>>>>>>> a811be87032c0952e61581daba100d9272a03409

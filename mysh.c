#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

// arbitrary value for the max command line characters

#define BUFF_SIZE 512

void batchMode(char *fName);
void interactiveMode();
void introTag(int key);
int execCommand(char *command);

int main(int argc, char *argv[])
{
    int FD;
    int flag = 0;
    ssize_t nBytes;
    char buffer[BUFF_SIZE];

    if (argc == 2)
    {
        // batch mode
        FD = open(argv[1], O_RDONLY);
        if (FD == -1)
        { // means there is an error in File directory
            printf("Error opening file - %s\n", argv[1]);
            exit(1);
        }
    }

    if (argc == 1)
    {
        // interactive mode
        FD = STDIN_FILENO;
        printf("Greetings...Welcome to mysh!\n");
        introTag(flag);
    }

    if (argc == 0 || argc > 2)
    {
        printf("this one Error\n");
        return EXIT_FAILURE;
    }

    /*parsing a sequence of commands that are separated
    by newline characters, and executing each command as soon as
    it is complete.
    */
    while ((nBytes = read(FD, buffer, BUFF_SIZE)) > 0)
    {

        if (buffer[nBytes - 1] == '\n')
            buffer[nBytes - 1] = '\0';

        // write(STDOUT_FILENO, buffer, nBytes);

        /* parsing the input command string into separate
        command tokens and then executing each
        token as a separate command by calling the
        */

        char *sepToken = strtok(buffer, "\n");

        while (sepToken != NULL)
        {

            flag = execCommand(sepToken);

            introTag(flag);

            // exit in file it will terminate
            if (strcmp(buffer, "exit") == 0)
            {
                printf("Exiting!\n");
                close(FD);
                return 0;
            }
            sepToken = strtok(NULL, "\n");
        }
    }

    if (nBytes == -1)
    {
        perror("Error reading file");
        exit(1);
    }

    close(FD);

    return 0;
}
//________________________________________________________________________________
int execCommand(char *command){
    char *arr[BUFF_SIZE];
    /*Initialize an array to hold the command arguments and a variable to keep
    track of the number of arguments:
    */
    int aCount = 0;
    char *token = strtok(command, " \t\n");

    while (token != NULL){
        // see if this works...not sure.

        if (strchr(token, '<') != NULL)
        {
            printf("%s\n", token);
        }

       if (strcmp(token, "*") == 0)
        {
            //wildcard
        }


        if (arr[0] == '/') {
        printf("%s is an absolute path\n", command);
        }

        if (strcmp(token, "exit") == 0)
        {
            //test this out
            printf("Exiting!\n");
            exit(1);
        }

        arr[aCount] = token;
        aCount++;
        token = strtok(NULL, " \t\n");
    }
/*The loop then continues to call strtok() with NULL as the first argument, which 
tells it to continue tokenizing the same string. It returns a pointer to the next 
token, or NULL if there are no more tokens. Each 
non-NULL token is stored in the args array and arg_count is incremented again.
*/
    arr[aCount] = NULL; // for execvp

    //**********************************************************************************

    if (strcmp(arr[0], "cd") == 0)
    {   
       if(arr[2]==NULL){

        chdir(arr[1]);

        perror("chdir");
        
       return 0;
       }
       else{
       printf("error too many cd args\n");
       //error condition stating wrong cd format more than one argument.
       exit(1);
       }
    }

    if(strcmp(arr[0],"pwd")==0){
        //pwd has no arguments thus 1 should be empty or NULL 
        if(arr[1]==NULL){

        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } 
        else{
            perror("pwd");
            exit(1);
        }
        return 0;  
    }
    else{
        printf("error too many pwd args\n");
        exit(1);
    }
}       

    pid_t pid = fork(); // child process

    if (pid == 0)
    {
        if (execvp(arr[0], arr) == -1)
        {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    else if (pid > 0)
    {
        /*blocks the parent process until the child
        process exits or is interrupted by a signal.*/
        int cPoint;
        waitpid(pid, &cPoint, 0);
        /*If the child process exited normally, we return
        its exit status using WEXITSTATUS().*/
        if (WIFEXITED(cPoint))
            return WEXITSTATUS(cPoint);

        else
            return -1;
    }

    else
    {
        perror("fork");
        return -1;
    }

    return 1;
}


//**************************************************************************
void introTag(int key){
    if (key != 0)
    {
        write(STDOUT_FILENO, "!mysh> ", 7);
    }

    else
    {
        write(STDOUT_FILENO, "mysh> ", 6);
    }
}



//___________________________________________________________________________
/*
void batchMode(char *fName)
{

    int FD = open(fName, O_RDONLY);

    if (FD == -1)
    { // means there is an error in File directory
        printf("Error opening file - %s\n", fName);
        exit(1);
    }

    ssize_t nBytes;

    char buffer[BUFF_SIZE];

    while ((nBytes = read(FD, buffer, BUFF_SIZE)) > 0)
    {
        if (buffer[nBytes - 1] == '\n')
        {
            buffer[nBytes - 1] = '\0';
        }
        // write(STDOUT_FILENO, buffer, nBytes);

        char *sepToken = strtok(buffer, "\n");

        while (sepToken != NULL)
        {

            int flag = execCommand(sepToken);

            introTag(flag);

            // exit in file it will terminate
            if (strcmp(buffer, "exit") == 0)
            {
                close(FD);
                return;
            }
            sepToken = strtok(NULL, "\n");
        }
    }

    if (nBytes == -1)
    {
        perror("Error reading file");
        exit(1);
    }

    close(FD);
}
//________________________________________________________________________________
void interactiveMode()
{
    printf("Greetings...Welcome to mysh!\n");

    // indicates to the print
    int flag = 0;

    while (1)
    {
        // print command
        introTag(flag);

        char buffer[BUFF_SIZE];
        // read to standard input
        int nBytes = read(STDIN_FILENO, buffer, BUFF_SIZE);

        if (nBytes == 0)
        {
            // exit while loop
            break;
        }
        // Remove trailing newline
        if (buffer[nBytes - 1] == '\n')
        {
            buffer[nBytes - 1] = '\0';
        }

        // if(strstr(buffer,"cd")!= NULL){
        //     cd(pathname);
        // }
        // if(strstr(buffer,"pwd")!= NULL){
        //     pwd();
        // }
        // Execute command
        // Make this command
        flag = execCommand(buffer);

        // Check for exit command
        if (strcmp(buffer, "exit\n") == 0)
        {
            return;
        }
    }
    printf("Exiting!\n");
}
//________________________________________________________________________________
*/
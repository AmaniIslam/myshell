#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <glob.h>
#include <sys/stat.h>

// arbitrary value for the max command line characters

#define BUFF_SIZE 1024

void batchMode(char *fName);
void interactiveMode();
void introTag(int key);
int execCommand(char *command);
int redirection(char **arr, int flag, char *file);
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

            if (argc == 1)
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
int execCommand(char *command)
{
    char *arr[BUFF_SIZE];
    /*Initialize an array to hold the command arguments and a variable to keep
    track of the number of arguments:
    */
    int aCount = 0;
    char *token = strtok(command, " \t\n");
    glob_t globbuf;
    int k;
    int flag = 0;
    // int pCheck=0;
    while (token != NULL)
    {
        // see if this works...not sure.

        if (token[0] == '>')
        {
            flag = 1;
            break;
        }

        if (token[0] == '<')
        {
            flag = 2;
            break;
        }

        // if (strcmp(token, "|") == 0){
        // pCheck=1;
        // pos=aCount;
        // }

        if (strchr(token, '*') != NULL)
        { // wildcard
            if (glob(arr[aCount], GLOB_NOCHECK | GLOB_TILDE, NULL, &globbuf) == 0)
            {
                for (k = 0; k < globbuf.gl_pathc; k++)
                {
                    arr[aCount + k] = globbuf.gl_pathv[k];
                }
                arr[aCount + k] = NULL;
                globfree(&globbuf);
            }
        }

        if (strcmp(token, "exit") == 0)
        {
            printf("Exiting!\n");
            exit(1);
        }

        arr[aCount] = token;
        aCount++;
        token = strtok(NULL, " \t\n");
    }
    arr[aCount] = NULL; // for execvp

    // if(flag!=0){
    // redirection(arr,flag,pos);
    // }
<<<<<<< HEAD
        // printf("\n\ntest1\n\n");
=======
>>>>>>> db0e877df980e3827f7cdd5785caa685a0683451

    //**********************************************************************************

    if (strcmp(arr[0], "cd") == 0)
    {
        if (arr[2] == NULL)
        {

            chdir(arr[1]);

            perror("chdir");

            return 0;
        }
        else
        {
            printf("error too many cd args\n");
            // error condition stating wrong cd format more than one argument.
            exit(1);
        }
    }

    if (strcmp(arr[0], "pwd") == 0)
    {
        // pwd has no arguments thus 1 should be empty or NULL
        if (arr[1] == NULL)
        {

            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s\n", cwd);
            }
            else
            {
                perror("pwd");
                exit(1);
            }
            return 0;
        }
        else
        {
            printf("error too many pwd args\n");
            exit(1);
        }
    }
<<<<<<< HEAD
}       
// if (arr[0][0] == '/')
// asprintf(&arr[0], "%s%s", ".", arr[0]);
    
    if(flag!=0){
    redirection(arr,flag,pos);
    return 0;
=======
    // if (arr[0][0] == '/')
    // asprintf(&arr[0], "%s%s", ".", arr[0]);

    if (flag != 0)
    {
        token = strtok(NULL, " \t\n");
        return redirection(arr, flag, token);
>>>>>>> db0e877df980e3827f7cdd5785caa685a0683451
    }

    printf("run\n");
    //********************************************************************************
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
int redirection(char **arr, int flag, char *file)
{
    if (flag == 1)
    {
        //>
        pid_t pid;
        // check first parameter
        int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
        if (fd < 0)
        {
            perror("open");
            return 1;
        }
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return 1;
        }
        else if (pid == 0)
        {
            if (dup2(fd, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }
<<<<<<< HEAD
            else if(pid==0){
                if(dup2(fd,STDOUT_FILENO)==-1){
                    perror("dup2");
                    exit(1);
                }
                // execlp(arr[0],arr[0],"-l",NULL);
                char *set[2]={"cat","amani.txt"};            
                execvp(arr[0], set);//fix

                perror("execvp");
                exit(EXIT_FAILURE);
            }
        else{
            waitpid(pid, NULL, 0);
=======
            execvp(arr[0], arr);

            perror("execvp");
            return 1;
>>>>>>> db0e877df980e3827f7cdd5785caa685a0683451
        }
        else
            waitpid(pid, NULL, 0);

        return 0;

        if (close(fd) == -1)
        {
            perror("close");
            return 1;
        }
    }
    else if (flag == 2)
    {
        //<
        int stat;
        pid_t pid;
<<<<<<< HEAD
        int fDir = open(arr[pos+1], O_RDONLY);

        if (fDir == -1) {
        perror("open");
        exit(1);
=======
        int fDir = open(file, O_RDONLY);
        if (fDir == -1)
        {
            perror("open");
            return 1;
>>>>>>> db0e877df980e3827f7cdd5785caa685a0683451
        }

        pid = fork();
<<<<<<< HEAD

        if (pid == -1) {
        perror("fork");
        exit(1);
=======
        if (pid == -1)
        {
            perror("fork");
            return 1;
>>>>>>> db0e877df980e3827f7cdd5785caa685a0683451
        }
        else if (pid == 0)
        { // child
            if (dup2(fDir, STDIN_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }
<<<<<<< HEAD
            char *set[2]={"cat","amani.txt"};            
            execvp(arr[0], set);//fix
=======
            execvp(arr[0], arr); // fix
>>>>>>> db0e877df980e3827f7cdd5785caa685a0683451
            perror("exec");
            return 1;
        }
        else
            waitpid(pid, &stat, 0);

        return 0;

        if (close(fDir) == -1)
        {
            perror("close");
            return 1;
        }
    }
    else
    {
        printf("Error");
<<<<<<< HEAD
        exit(1); 
    
=======
        return 1;
    }
    return 1;
>>>>>>> db0e877df980e3827f7cdd5785caa685a0683451
}

/*
errors that need to be fixed
cat: invalid option -- 'l'
Try 'cat --help' for more information.
cat: '<': No such file or directory
hello
!mysh>
*/

//**************************************************************************
void introTag(int key)
{
    if (key != 0)
    {
        write(STDOUT_FILENO, "!mysh> ", 7);
    }

    else
    {
        write(STDOUT_FILENO, "mysh> ", 6);
    }
}
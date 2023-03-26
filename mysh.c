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
#define _GNU_SOURCE

void batchMode(char *);
void interactiveMode();
void introTag(int);
int piper(char **, char **);
int execCommand(char *);
int redirection(char **, int, char *);
int wildCard(char **, int, int);

int main(int argc, char *argv[])
{
    int FD;
    int flag = 0;
    int pos = 0;
    ssize_t nBytes;
    char buffer[BUFF_SIZE];
    char line[BUFF_SIZE];

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
        printf("Greetings...Welcome to my shell!\n");
        introTag(flag);
    }

    if (argc == 0 || argc > 2)
    {
        printf("Too many or too few arguments.\n");
        return EXIT_FAILURE;
    }

    /*parsing a sequence of commands that are separated
    by newline characters, and executing each command as soon as
    it is complete.
    */

    while ((nBytes = read(FD, buffer, BUFF_SIZE)) > 0)
    {
        for (int i = 0; i < nBytes; i++)
        {
            if (buffer[i] == '\n')
            {
                line[pos] = '\0';
                printf("%s\n", line);
                execCommand(line);
                pos = 0;
            }
            else
                line[pos++] = buffer[i];
        }
    }
    // write(STDOUT_FILENO, buffer, nBytes);

    /* parsing the input command string into separate
    command tokens and then executing each
    token as a separate command by calling the
    */
    /*
            char *sepToken = strtok(buffer, "\n");

            printf("tok: %s/\n", sepToken);

            while (sepToken != NULL)
            {
                flag = execCommand(sepToken);

                if (argc == 1)
                    introTag(flag);

                // exit in file it will terminate
                */

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
    // printf("%s\n", command);
    char *arr[BUFF_SIZE];
    /*Initialize an array to hold the command arguments and a variable to keep
    track of the number of arguments:
    */
    int aCount = 0;
    char *token = strtok(command, " \t\n");
    int flag = 0;
    int pos = 0;
    int count1 =0;
    int count2 = 0; 
    int wCard = 0;

    // int pipefd[2];
    // pid_t pid1, pid2;
    // int status;
    char *sub[BUFF_SIZE];
    int subCount = 0;
    int subCom = 0;

    while (token != NULL)
    {
        // see if this works...not sure.
        if (token[0] == '>')
        {
            count1++;
            flag = 1;
            break;
        }

        if (token[0] == '<')
        {
            count2++;
            flag = 2;
            break;
        }

        if (strchr(token, '*') != NULL)
        { // wildcard
            pos = aCount;
            wCard = 1;
        }

        if (token[0] == '|')
        {
            token = strtok(NULL, " \t\n");
            while (token != NULL)
            {
                sub[subCount] = token;
                subCount++;
                token = strtok(NULL, " \t\n");
            }
            subCom = 1;
            return piper(arr, sub);
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

<<<<<<< HEAD
    if( count1>1 || count2 >1 ){
        //too many file redirections. 
        exit(1);
    }
    //**********************************************************************************
    // if(arr =='*' && arr [][] == '/'&&){

    // }
    //   */*.txt
    // 3.3 extension
    // strlen(arr[i])>2 &&

=======
>>>>>>> 9d7518d944ec91c8c516d046c5689bb26afa443f
    for (int i = 0; i < aCount; i++)
        for (int k; k < strlen(arr[i]); k++)
        {
            if (strlen(arr[i]) > 2 && arr[i][k] == '*' && arr[i][k + 1] == '/' && arr[i][k + 2] == '*')
                return wildCard(arr, i, 1);
        }

    if (wCard != 0)
        return wildCard(arr, pos, 0);

    if (arr[0][0] == '~' && (arr[0][1] == '\0' || arr[0][1] == '/'))
    {
        // extention 3.2            : ~/
        char homePath[BUFF_SIZE];
        const char *homeDirectory = getenv("HOME");
        if (homeDirectory == NULL)
        {
            fprintf(stderr, "Error: HOME environment variable not set\n");
            return -1;
        }
        snprintf(homePath, BUFF_SIZE, "%s%s", homeDirectory, *arr + 1);
        int err = chdir(homePath);
        if (err != 0)
        {
            perror("chdir");
            return 2;
        }
        return 0;
    }

    if (strcmp(arr[0], "cd") == 0)
    {
        if (arr[1] == NULL)
        {
            // extention 3.2
            char homePath[BUFF_SIZE];
            const char *homeDirectory = getenv("HOME");
            if (homeDirectory == NULL)
            {
                fprintf(stderr, "Error: HOME environment variable not set\n");
                return -1;
            }

            strcpy(homePath, homeDirectory);

            int err = chdir(homePath);
            if (err != 0)
            {
                perror("chdir");
                return 2;
            }
            return 0;
            // WORKING ON REMAINING>
        }
        else if (arr[2] == NULL)
        {

            chdir(arr[1]);

            perror("chdir");

            return 0;
        }
        else
        {
            printf("error too many cd args\n");
            return 1;
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
   if (arr[0][0] == '/')
=======

    if (arr[0][0] == '/')
>>>>>>> 9d7518d944ec91c8c516d046c5689bb26afa443f
    {
        char *new_str = malloc(strlen(arr[0]) + 2);
        new_str[0] = '.';
        strcpy(new_str + 1, arr[0]);
        arr[0] = new_str;
    }
<<<<<<< HEAD

=======
>>>>>>> 9d7518d944ec91c8c516d046c5689bb26afa443f

    if (flag != 0)
    {
        token = strtok(NULL, " \t\n");
        return redirection(arr, flag, token);
    }

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
        return 1;
    }

    return 1;
}

int piper(char **first, char **second)
{

    int pipefd[2];
    pid_t pid1, pid2;
    int status;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid1 = fork();
    if (pid1 == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid1 == 0)
    {
        // Child process 1
        close(pipefd[0]);               // Close read end of pipe
        dup2(pipefd[1], STDOUT_FILENO); // Set stdout to write end of pipe
        execvp(first[0], first);        // Replace process image with command1
        perror("exec command1");
        exit(1);
    }

    pid2 = fork();
    if (pid2 == -1)
    {
        perror("fork");
        return 1;
    }
    else if (pid2 == 0)
    {
        // Child process 2
        close(pipefd[1]);              // Close write end of pipe
        dup2(pipefd[0], STDIN_FILENO); // Set stdin to read end of pipe
        execvp(second[0], second);     // Replace process image with command2
        perror("exec command2");
        exit(1);
    }

    // Parent process
    close(pipefd[0]); // Close read end of pipe in parent
    close(pipefd[1]); // Close write end of pipe in parent
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    else
        return 1; // Error
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
            execvp(arr[0], arr);

            perror("execvp");
            return 1;
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
        int fDir = open(file, O_RDONLY);
        if (fDir == -1)
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
        { // child
            if (dup2(fDir, STDIN_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }
            execvp(arr[0], arr); // fix
            perror("exec");
            return 1;
        }
        else
            waitpid(pid, &stat, 0);

        if (close(fDir) == -1)
        {
            perror("close");
            return 1;
        }

        return 0;
    }
    else
    {
        printf("Error");
        return 1;
    }
    return 1;
}
//**************************************************************************
int wildCard(char **arr, int pos, int exCheck)
{
    if (exCheck == 1)
    {
        // 3.3 extension portion.
        // need to double check this ...
        char *reserve[256];
        int count = 0;
        char *p = strtok(arr[pos], "/");
        while (p != NULL)
        {
            reserve[count++] = p; // store each segment in the array and increment the count
            p = strtok(NULL, "/");
        }
        char token[1024] = "";

        for (int i = 0; i < count; i++)
        {
            if (strchr(reserve[i], '*') != NULL)
            {
                strcat(token, "/*");
                strcat(token, reserve[i]);
                strcat(token, "*/");
            }
            else
            {
                strcat(token, "/");
                strcat(token, reserve[i]);
            }
        }
        strcat(token, "*");
    }

    int k;
    glob_t globbuf;

    int val = glob(arr[pos], GLOB_NOCHECK | GLOB_TILDE, NULL, &globbuf);

    if (val != 0)
    {
        printf("Error: glob() failed \n");
        return 1;
    }

    for (k = 0; k < globbuf.gl_pathc; k++)
    {
        printf("%s\n", globbuf.gl_pathv[k]);
    }
    globfree(&globbuf);

    return 0;
}
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

// int main(int argc, char *argv[]) {
//     char *path_env = getenv("PATH");
//     char path[MAX_PATH_LENGTH];
//     char *dir;
//     struct stat statbuf;
//     int i, found = 0;

//     if (argc < 2) {
//         fprintf(stderr, "Usage: %s command [arg...]\n", argv[0]);
//         return 1;
//     }

//     /* Check each directory in PATH for the command */
//     while ((dir = strsep(&path_env, ":")) != NULL) {
//         snprintf(path, MAX_PATH_LENGTH, "%s/%s", dir, argv[1]);
//         if (stat(path, &statbuf) == 0) {  /* Check if file exists */
//             if (S_ISREG(statbuf.st_mode) && (statbuf.st_mode & S_IXUSR)) {
//                 /* File exists and is executable */
//                 found = 1;
//                 break;
//             } else {
//                 /* File exists but is not executable */
//                 fprintf(stderr, "%s: %s: Permission denied\n", argv[0], path);
//                 return 1;
//             }
//         }
//     }

//     /* Execute the command if found, otherwise print an error message */
//     if (found) {
//         execv(path, &argv[1]);
//         fprintf(stderr, "%s: %s: Execution failed\n", argv[0], path);
//         return 1;
//     } else {
//         fprintf(stderr, "%s: %s: Command not found\n", argv[0], argv[1]);
//         return 1;
//     }
// }
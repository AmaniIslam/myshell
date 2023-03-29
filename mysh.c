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

// void batchMode(char *);
// void interactiveMode();
void introTag(int);
int re_pipe(char **, char **, char *, char *, char *);
int execCommand(char *,int);
int redirection(char **, int, char *);
// char** wildCard(char **, int, int,int);
int pwd_cd(char **);


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
            return 1;
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
                if (argc == 2)
                    execCommand(line,FD);
                if (argc == 1)
                    introTag(execCommand(line,FD));
                pos = 0;
            }
            else
                line[pos++] = buffer[i];
        }
    }
    if (nBytes == -1)
    {
        perror("Error reading file");
        return 1;
    }

    close(FD);

    return 0;
}
//________________________________________________________________________________
int execCommand(char *command,int mode)
{
    // printf("%s\n", command);
    int returnVal=0; 
    char *arr[BUFF_SIZE];
    /*Initialize an array to hold the command arguments and a variable to keep
    track of the number of arguments:
    */
    int aCount = 0;
    char *token = strtok(command, " \t\n");
    int pos = 0;
    int wCard = 0;
    char *input = NULL;
    char *output = NULL;
    char *subOutput = NULL;
    char *sub[BUFF_SIZE];
    int sCount = 0;
    char *space[1];

    // int sCard = 0;

    
    while (token != NULL)
    {
        if (token[0] == '>')
        {
            if (output != NULL)
                return 1;
            output = strtok(NULL, " \t\n");
            token = strtok(NULL, " \t\n");
            continue;
        }

        if (token[0] == '<')
        {
            if (input != NULL)
                return 1;
            input = strtok(NULL, " \t\n");
            token = strtok(NULL, " \t\n");
            continue;
        }

        if (strchr(token, '*') != NULL)
        { // wildcard
            pos = aCount;//assigning the position to pos
            wCard = 1;
        }

        if (token[0] == '|')
        {
            token = strtok(NULL, " \t\n");
            while (token != NULL)
            {

                if (output != NULL || token[0] == '<')
                    return 1;

                if (token[0] == '>')
                {
                    if (subOutput != NULL)
                        return 1;
                    subOutput = strtok(NULL, " \t\n");
                    token = strtok(NULL, " \t\n");
                    continue;
                }

                sub[sCount] = token;
                sCount++;
                token = strtok(NULL, " \t\n");
            }
            sub[sCount] = NULL;
            // sCard = 1;
            break;
        }

        arr[aCount] = token;
        aCount++;
        token = strtok(NULL, " \t\n");
    }
    arr[aCount] = NULL; // for execvp


    for (int i = 0; i < aCount; i++){
        for (int k=0; k < strlen(arr[i]); k++){
            
            if (strlen(arr[i]) > 2 && arr[i][k] == '*' && arr[i][k + 1] == '/' && arr[i][k + 2] == '*'){
            wCard=1; 
            }
        }
    }
//wildcard
    glob_t globbuf;

    if (wCard != 0){
        char *copyArr[BUFF_SIZE];
        int dup = pos; 
        for (int i = 0; i < aCount; i++){
        copyArr[i] = (char*) malloc(strlen(arr[i]) + 1);
            for (int k=0; k < strlen(arr[i]); k++){
                copyArr[i][k]=arr[i][k];
            }
        copyArr[i][strlen(arr[i])] = '\0';

        }
            arr[aCount] = 0; 
        int k;

        int val = glob(arr[pos], GLOB_NOCHECK | GLOB_TILDE, NULL, &globbuf);
        int count=0; 

        if (val != 0){
            printf("Error: glob() failed \n");
            exit(1);
        }
        if(val == GLOB_NOMATCH){
            arr[aCount] = NULL;
            return 1; 
        }
        if(val==0){
            for (k = 0; k < globbuf.gl_pathc; k++){
            // printf("%s\n", globbuf.gl_pathv[k]);
            arr[pos] = globbuf.gl_pathv[k];
            pos++;      
            count++;
        } 
        if(strcmp(arr[dup],copyArr[dup])==0){
        printf("Error no match\n");
        for (int i = 0; i < aCount; i++) {
        free(copyArr[i]);
        }
        globfree(&globbuf); 
        return 1; 
            }
        for (int i = 0; i < aCount; i++) {
        free(copyArr[i]);
        }
        }
    }

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

    if (arr[0][0] == '/')
    {
        char *new_str = malloc(strlen(arr[0]) + 2);
        new_str[0] = '.';
        strcpy(new_str + 1, arr[0]);
        arr[0] = new_str;
        //may need to be freed
    }

    if (sub[0] != NULL && sub[0][0] == '/')
    {
        char *new_str = malloc(strlen(sub[0]) + 2);
        new_str[0] = '.';
        strcpy(new_str + 1, sub[0]);
        sub[0] = new_str;   
    }

    if (sub[0] != NULL)
    {
        if (strcmp(arr[0], "exit") == 0)
        {
            if ((strcmp(sub[0], "cd") == 0 || strcmp(sub[0], "pwd") == 0) && subOutput == NULL)
                pwd_cd(sub);
            else
                re_pipe(sub, space, subOutput, input, subOutput);
            exit(1);
        }

        if (sub[0] != NULL && strcmp(sub[0], "exit") == 0)
        {
            if (strcmp(arr[0], "cd") == 0 || (strcmp(arr[0], "pwd") == 0 && output == NULL))
                pwd_cd(arr);
            else
                re_pipe(arr, space, output, input, subOutput);
            exit(1);
        }

        if (strcmp(arr[0], "cd") == 0)
        {
            if (strcmp(sub[0], "cd") == 0 || (strcmp(sub[0], "pwd") == 0 && subOutput == NULL)){
                returnVal= pwd_cd(arr) + pwd_cd(sub);
                if(wCard!=0)
                globfree(&globbuf);
                return returnVal;
                }
            returnVal= pwd_cd(arr) + re_pipe(sub, space, subOutput, input, subOutput);
            if(wCard!=0){
            globfree(&globbuf);}
            return returnVal;
        }

        if (strcmp(sub[0], "cd") == 0)
        {
            if (strcmp(arr[0], "pwd") == 0){
                returnVal= pwd_cd(arr) + pwd_cd(sub);
                if(wCard!=0){
                globfree(&globbuf);}
                return returnVal;
            }

            returnVal = re_pipe(arr, space, output, input, subOutput) + pwd_cd(sub);
            if(wCard!=0){
            globfree(&globbuf);}
            return returnVal;
        }

        returnVal = re_pipe(arr, sub, output, input, subOutput);
        if(wCard!=0){
        globfree(&globbuf);}
        return returnVal;
    }

    if (strcmp(arr[0], "exit") == 0){//check this for works everytime make sure
        if(mode == STDIN_FILENO){
            printf("Exiting!\n");
        }
        exit(1);
    }

    if (strcmp(arr[0], "cd") == 0 || (strcmp(arr[0], "pwd") == 0 && output == NULL)){
        returnVal= pwd_cd(arr);
        if(wCard!=0){
        globfree(&globbuf);}
        return returnVal;
    }
    // return redirection(arr, 2, input);
    //        printf("test%d\n", strcmp(sub[0], "exit"));
    
    returnVal = re_pipe(arr, sub, output, input, subOutput);
    if(wCard!=0){
    globfree(&globbuf);}
    return returnVal;

    //********************************************************************************
    /*
    pid_t pid = fork(); // child process

    if (pid == 0)
    {
        if (execvp(arr[0], arr) == -1)
        {
            perror("execvp");
            return 1;
        }
    }

    else if (pid > 0)
    {
        //blocks the parent process until the child process exits or is interrupted by a signal.
    int cPoint;
    waitpid(pid, &cPoint, 0);
    //If the child process exited normally, we return its exit status using WEXITSTATUS().
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
*/

}

int re_pipe(char **first, char **second, char *output, char *input, char *subOutput)
{
    int pipefd[2];
    pid_t pid1, pid2;
    int fd;
    int fDir;
    int stat;

    if (second[0] != NULL && pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid1 = fork();
    if (pid1 == -1)
    {
        perror("fork");
        return 1;
    }
    else if (pid1 == 0)
    {
        // Child process 1

        if (output != NULL)
        {
            //>
            fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
            if (fd < 0)
            {
                perror("open");
                return 1;
            }

            if (dup2(fd, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }
        }

        if (input != NULL)
        {
            //<

            fDir = open(input, O_RDONLY);
            if (fDir == -1)
            {
                perror("open");
                return 1;
            }
            if (dup2(fDir, STDIN_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }
        }

        if (second[0] != NULL)
        {

            // No redirection
            close(pipefd[0]); // Close read end of pipe
            if (dup2(pipefd[1], STDOUT_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }
        }

        if (execvp(first[0], first) == -1)
        {
            perror("exec command1");
            return 1;
        }
    }

    if (second[0] != NULL)
    {
        pid2 = fork();
        if (pid2 == -1)
        {
            perror("fork");
            return 1;
        }
        else if (pid2 == 0)
        {
            // Child process 2
            if (subOutput != NULL)
            {
                fd = open(subOutput, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
                if (fd < 0)
                {
                    perror("open");
                    return 1;
                }
                if (dup2(fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    return 1;
                }
            }
            // No redirection
            close(pipefd[1]); // Close write end of pipe
            if (dup2(pipefd[0], STDIN_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }

            if (execvp(second[0], second) == -1)
            {
                perror("exec command2");
                return 1;
            }

            return 0;
        }

        // Parent process
        close(pipefd[0]); // Close read end of pipe in parent
        close(pipefd[1]); // Close write end of pipe in parent
        waitpid(pid1, &stat, 0);
        waitpid(pid2, &stat, 0);
    }

    else
        waitpid(pid1, &stat, 0);

    return 0;
}

/*
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
            return 0;
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
            return 0;
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
*/
//**************************************************************************
// char** wildCard(char **arr, int pos, int exCheck,int aCount)
// {
//     if (exCheck == 1)
//     {
//         // 3.3 extension portion.
//         // need to double check this ...
//         char *reserve[256];
//         int count = 0;
//         char *p = strtok(arr[pos], "/");
//         while (p != NULL)
//         {
//             reserve[count++] = p; // store each segment in the array and increment the count
//             p = strtok(NULL, "/");
//         }
//         char token[1024] = "";

//         for (int i = 0; i < count; i++)
//         {
//             if (strchr(reserve[i], '*') != NULL)
//             {
//                 strcat(token, "/*");
//                 strcat(token, reserve[i]);
//                 strcat(token, "*/");
//             }
//             else
//             {
//                 strcat(token, "/");
//                 strcat(token, reserve[i]);
//             }
//         }
//         strcat(token, "*");
//     }

//     int k;
//     glob_t globbuf;

//     int val = glob(arr[pos], GLOB_NOCHECK | GLOB_TILDE, NULL, &globbuf);
//     int count=0; 

//     if (val != 0){
//         printf("Error: glob() failed \n");
//         exit(1);
//     }
//     else if(val==0){
        
//         for (k = 0; k < globbuf.gl_pathc; k++){
//         // printf("%s\n", globbuf.gl_pathv[k]);
//         arr[pos] = globbuf.gl_pathv[k];
//         // printf("arr pos val: %s\n",arr[pos]);
//         pos++;
//         count++;
//         } 

//         printf("\narr pos final val: %s\n",arr[pos]);

//         globfree(&globbuf); 
//     }
//     else {//no match
//         arr[aCount] = NULL;
//     }

//     return arr;
// }
//**************************************************************************
void introTag(int key)
{
    if (key != 0)
        write(STDOUT_FILENO, "!mysh> ", 7);

    else
        write(STDOUT_FILENO, "mysh> ", 6);
}

int pwd_cd(char **arr)
{
    if (strcmp(arr[0], "pwd") == 0)
    {
        // pwd has no arguments thus 1 should be empty or NULL
        if (arr[1] == NULL)
        {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) == NULL)
            {
                perror("pwd");
                return 1;
            }

            printf("%s\n", cwd);
            return 0;
        }

        printf("error too many pwd args\n");
        return 1;
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
                return 1;
            }

            strcpy(homePath, homeDirectory);

            if (chdir(homePath) != 0)
            {
                perror("chdir");
                return 1;
            }
            return 0;
            // WORKING ON REMAINING>
        }
        else if (arr[2] == NULL)
        {
            if (chdir(arr[1]) != 0)
            {
                perror("chdir");
                return 1;
            }

            return 0;
        }
    
        printf("error too many cd args\n");
        return 1;
    }

    return 0;
}
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
void introTag(int);
int re_pipe(char **, char **, char *, char *, char *);
int execCommand(char *, int);
int redirection(char **, int, char *);
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
                    execCommand(line, FD);
                if (argc == 1)
                    introTag(execCommand(line, FD));
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
int execCommand(char *command, int mode)
{
    // printf("%s\n", command);
    int returnVal = 0;
    char *arr[BUFF_SIZE];
    /*Initialize an array to hold the command arguments and a variable to keep
    track of the number of arguments:
    */
    int sCard = 0;
    int aCount = 0;
    char *token = strtok(command, " \t\n");
    int pos = 0;
    int sPos = 0;
    int wCard = 0;
    char *input = NULL;
    char *output = NULL;
    char *subOutput = NULL;
    char *sub[BUFF_SIZE];
    int sCount = 0;
    char *space[1];

    memset(arr, 0, sizeof(arr));
    memset(sub, 0, sizeof(sub));

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
        {                 // wildcard
            pos = aCount; // assigning the position to pos
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

                if (strchr(token, '*') != NULL)
                {                  // wildcard
                    sPos = sCount; // assigning the position to pos
                    sCard = 1;
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

    for (int i = 0; i < aCount; i++)
    {
        for (int k = 0; k < strlen(arr[i]); k++)
        {

            if (strlen(arr[i]) > 2 && arr[i][k] == '*' && arr[i][k + 1] == '/' && arr[i][k + 2] == '*')
                wCard = 1;
        }
    }
    // wildcard
    glob_t globbuf, subglob;

    if (wCard != 0)
    {
        char *copyArr[BUFF_SIZE];
        int dup = pos;
        for (int i = 0; i < aCount; i++)
        {
            copyArr[i] = (char *)malloc(strlen(arr[i]) + 1);
            for (int k = 0; k < strlen(arr[i]); k++)
            {
                copyArr[i][k] = arr[i][k];
            }
            copyArr[i][strlen(arr[i])] = '\0';
        }
        arr[aCount] = 0;
        int k;

        int val = glob(arr[pos], GLOB_NOCHECK | GLOB_TILDE, NULL, &globbuf);
        int count = 0;

        if (val != 0)
        {
            printf("Error: glob() failed \n");
            exit(1);
        }
        if (val == GLOB_NOMATCH)
        {
            arr[aCount] = NULL;
            return 1;
        }
        if (val == 0)
        {
            for (k = 0; k < globbuf.gl_pathc; k++)
            {
                // printf("%s\n", globbuf.gl_pathv[k]);
                arr[pos] = globbuf.gl_pathv[k];
                pos++;
                count++;
            }
            if (strcmp(arr[dup], copyArr[dup]) == 0)
            {
                printf("Error no match\n");
                for (int i = 0; i < aCount; i++)
                {
                    free(copyArr[i]);
                }
                globfree(&globbuf);
                return 1;
            }
            for (int i = 0; i < aCount; i++)
                free(copyArr[i]);
        }
    }

    if (sCard != 0)
    {
        char *copyArr[BUFF_SIZE];
        int dup = sPos;
        for (int i = 0; i < sCount; i++)
        {
            copyArr[i] = (char *)malloc(strlen(sub[i]) + 1);
            for (int k = 0; k < strlen(sub[i]); k++)
            {
                copyArr[i][k] = sub[i][k];
            }
            copyArr[i][strlen(sub[i])] = '\0';
        }
        sub[sCount] = 0;
        int k;

        int val = glob(sub[sPos], GLOB_NOCHECK | GLOB_TILDE, NULL, &subglob);
        int count = 0;

        if (val != 0)
        {
            printf("Error: glob() failed \n");
            exit(1);
        }
        if (val == GLOB_NOMATCH)
        {
            sub[sCount] = NULL;
            return 1;
        }
        if (val == 0)
        {
            for (k = 0; k < subglob.gl_pathc; k++)
            {
                // printf("%s\n", globbuf.gl_pathv[k]);
                sub[sPos] = subglob.gl_pathv[k];
                sPos++;
                count++;
            }
            if (strcmp(sub[dup], copyArr[dup]) == 0)
            {
                printf("Error no match\n");
                for (int i = 0; i < sCount; i++)
                    free(copyArr[i]);

                globfree(&subglob);
                return 1;
            }
            for (int i = 0; i < sCount; i++)
                free(copyArr[i]);
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
    int path = 0;
    if (arr[0][0] == '/')
    {
        path = 1;
        char *new_str = malloc(strlen(arr[0]) + 2);
        new_str[0] = '.';
        strcpy(new_str + 1, arr[0]);
        arr[0] = new_str;
        // may need to be freed
    }
    int sPath = 0;
    if (sub[0] != NULL && sub[0][0] == '/')
    {
        sPath = 1;
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
            {
                re_pipe(sub, space, subOutput, input, subOutput);
                if (sCard != 0)
                    globfree(&subglob);
                if (sPath)
                    free(sub[0]);
            }
            exit(1);
        }

        if (sub[0] != NULL && strcmp(sub[0], "exit") == 0)
        {
            if (strcmp(arr[0], "cd") == 0 || (strcmp(arr[0], "pwd") == 0 && output == NULL))
                pwd_cd(arr);
            else
            {
                re_pipe(arr, space, output, input, subOutput);
                if (wCard != 0)
                    globfree(&globbuf);
                if (path)
                    free(arr[0]);
            }
            exit(1);
        }

        if (strcmp(arr[0], "cd") == 0)
        {
            if (strcmp(sub[0], "cd") == 0 || (strcmp(sub[0], "pwd") == 0 && subOutput == NULL))
                return pwd_cd(arr) + pwd_cd(sub);

            returnVal = pwd_cd(arr) + re_pipe(sub, space, subOutput, input, subOutput);

            if (sCard != 0)
                globfree(&subglob);
            if (sPath)
                free(sub[0]);
            return returnVal;
        }

        if (strcmp(sub[0], "cd") == 0)
        {
            if (strcmp(arr[0], "pwd") == 0)
                return pwd_cd(arr) + pwd_cd(sub);

            returnVal = re_pipe(arr, space, output, input, subOutput) + pwd_cd(sub);
            if (wCard != 0)
                globfree(&globbuf);
            if (path)
                free(arr[0]);
            return returnVal;
        }

        returnVal = re_pipe(arr, sub, output, input, subOutput);
        if (wCard != 0)
            globfree(&globbuf);
        if (sCard != 0)
            globfree(&subglob);
        if (path)
            free(arr[0]);
        if (sPath)
            free(sub[0]);
        return returnVal;
    }

    if (strcmp(arr[0], "exit") == 0)
    { // check this for works everytime make sure
        if (mode == STDIN_FILENO)
            printf("Exiting!\n");

        exit(1);
    }

    if (strcmp(arr[0], "cd") == 0 || (strcmp(arr[0], "pwd") == 0 && output == NULL))
        return pwd_cd(arr);

    returnVal = re_pipe(arr, sub, output, input, subOutput);
    if (wCard != 0)
        globfree(&globbuf);
    if (sCard != 0)
        globfree(&subglob);
    if (path)
        free(arr[0]);
    if (sPath)
        free(sub[0]);
    return returnVal;
}

/*
int re_pipe(char **first, char **second, char *output, char *input, char *subOutput)
{
    if (second[0] != NULL)
        printf("%s\n", second[0]);

    int pipefd[2];
    int fd_in = STDIN_FILENO;
    int fd_out = STDOUT_FILENO;

    if (input != NULL)
    {
        fd_in = open(input, O_RDONLY);
        if (fd_in < 0)
        {
            perror("open");
            return -1;
        }
    }

    if (output != NULL)
    {
        fd_out = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd_out < 0)
        {
            perror("open");
            return -1;
        }
    }

    if (subOutput != NULL)
    {
        int fd_subOut = open(subOutput, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd_subOut < 0)
        {
            perror("open");
            return -1;
        }
        dup2(fd_subOut, STDERR_FILENO);
        close(fd_subOut);
    }

    if (second[0] != NULL)
    {
        if (pipe(pipefd) < 0)
        {
            perror("pipe");
            return -1;
        }
    }

    pid_t pid = fork();

    if (pid == 0)
    {

        if (input != NULL)
        {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (output != NULL)
        {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        if (second != NULL)
        {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        }

        execvp(first[0], first);
        perror("execvp1");
    }

    if (second[0] != NULL)
    {
        pid_t pid2 = fork();

        if (pid2 == 0)
        {
            if (output != NULL)
            {
                int fd_out2 = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd_out2 < 0)
                {
                    perror("open");
                    return -1;
                }
                dup2(fd_out2, STDOUT_FILENO);
                close(fd_out2);
            }

            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            execvp(second[0], second);
            perror("execvp2");
        }

        close(pipefd[0]);
        close(pipefd[1]);

        waitpid(pid2, NULL, 0);
    }

    waitpid(pid, NULL, 0);
    return 0;
}
*/

int re_pipe(char **first, char **second, char *output, char *input, char *subOutput)
{
    int pipefd[2];
    pid_t pid1, pid2;
    int fd;
    int fDir;

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
            close(fd);
        }

        if (input != NULL)
        {
            //<

            fDir = open(input, O_RDONLY);
            if (fDir == -1)
            {
                printf("%s\n", output);
                perror("open");
                return 1;
            }
            if (dup2(fDir, STDIN_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }
            close(fDir);
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
            close(pipefd[1]);
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
            // Child process 1

            if (subOutput != NULL)
            {
                //>
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
                close(fd);
            }
            // No redirection
            close(pipefd[1]); // Close read end of pipe
            if (dup2(pipefd[0], STDIN_FILENO) == -1)
            {
                perror("dup2");
                return 1;
            }
            close(pipefd[0]);

            if (execvp(second[0], second) == -1)
            {
                perror("exec command1");
                return 1;
            }
        }
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid2, NULL, 0);
    }
    waitpid(pid1, NULL, 0);
    return 0;
}

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
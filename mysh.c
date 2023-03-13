#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

//arbitrary value for the max command line characters

#define BUFF_SIZE 1024

void batchMode(char *fName);
void interactiveMode();
void introTag(int key);
int execCommand(char* command);

int main(int argc, char *argv[]){   
    if(argc==2){
        //batchmode
        //passes in file name to runBatch
        batchMode(argv[1]);
    }
    if(argc==1){
        //interactive mode
        interactiveMode();
    }
    if(argc==0 || argc > 2){
        printf("Error\n");
        return EXIT_FAILURE;
    }

    return 0; 
}
//________________________________________________________________________________
int execCommand(char* command){
    char * arr[BUFF_SIZE];
/*Initialize an array to hold the command arguments and a variable to keep 
track of the number of arguments:
*/
    int aCount = 0;

    char * token = strtok(command, " \t\n");


    while(token!=NULL){
        // if(token=="<"){

        // }
        char* ptr = strchr(token, '<');
        
        if (ptr != NULL) {
        printf("Found\n");
        }

        arr[aCount]=token;
        aCount++;
        token = strtok(NULL, " \t\n");
    }
    
    arr[aCount] = NULL;  // for execvp 

  pid_t pid = fork();//child process

return 1; 
}


//________________________________________________________________________________
void batchMode(char *fName){
    
    int FD = open(fName,O_RDONLY);

    if(FD==-1){//means there is an error in File directory 
    printf("Error opening file - %s\n",fName);
    exit(1);
    }

    ssize_t nBytes;

    char buffer[BUFF_SIZE];

    /*parsing a sequence of commands that are separated 
    by newline characters, and executing each command as soon as 
    it is complete.
    */
    while((nBytes=read(FD,buffer,BUFF_SIZE))>0){
            if(buffer[nBytes-1]=='\n'){
                buffer[nBytes-1]='\0';
            }    
        //write(STDOUT_FILENO, buffer, nBytes);

    /* parsing the input command string into separate 
    command tokens and then executing each 
    token as a separate command by calling the 
    */
        char *sepToken = strtok(buffer,"\n");

        while(sepToken!=NULL){
            
            int flag = execCommand(sepToken);

            introTag(flag);

        //exit in file it will terminate
        if (strcmp(buffer, "exit") == 0){
        close(FD);
        return;
            }
            sepToken = strtok(NULL, "\n");
        }
    }
        
    if (nBytes == -1) {
    perror("Error reading file");
    exit(1);
    }

    close(FD);
}
//________________________________________________________________________________
void interactiveMode(){
    printf("Greetings...Welcome to mysh!\n");

    //indicates to the print
    int flag=0;

    while(1){
        //print command
        introTag(flag); 

        char buffer[BUFF_SIZE];
        //read to standard input
        int nBytes = read(STDIN_FILENO, buffer, BUFF_SIZE);

        if(nBytes==0){
            //exit while loop
            break;
        }
                // Remove trailing newline
        if (buffer[nBytes-1] == '\n') {
            buffer[nBytes-1] = '\0';
        }

        // Execute command
        //Make this command
        //currPoint = execute_command(buffer);

        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            return;
        }
    }
    
    printf("Bye!\n");

}
//________________________________________________________________________________
void introTag(int key){
    if (key != 0) {
        write(STDOUT_FILENO, "!mysh> ", 7);
    } 
    
    else {
        write(STDOUT_FILENO, "mysh> ", 6);
    }
}
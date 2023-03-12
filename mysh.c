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
    if(argc==0){
        printf("No arguments passed in");
        return EXIT_FAILURE;
    }

    return 0; 
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
                //function call to parse the commands  
                //right here
            }
    
        //write(STDOUT_FILENO, buffer, nBytes);

        //buffer command is the word exit
        if (strcmp(buffer, "exit\n") == 0){
            close(FD);
            return;
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
    printf("Welcome to mysh!\n");

    //indicates to the print
    int currPoint=0;

    while(1){
        //print command
        introTag(currPoint); 

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
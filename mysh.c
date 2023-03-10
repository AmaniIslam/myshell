#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

    //arbitrary value for the max command line characters
    #define BUFF_SIZE 1024

void runBatch(char *fName);

int main(int argc, char *argv[]){   
    if(argc==2){
        //batchmode
        //passes in file name to runBatch
        runBatch(argv[1]);
    }
    if(argc==1){
        //interactive mode
    }
    if(argc==0){
        printf("No arguments passed in");
        return EXIT_FAILURE;
    }

    return 0; 
}

void runBatch(char *fName){
    
    int FD = open(fName,O_RDONLY);

    if(FD==-1){//means there is an error in File directory 
    printf("Error opening file - %s\n",fName);
    exit(1);
    }

    ssize_t bytes;

    char buffer[BUFF_SIZE];

    /*parsing a sequence of commands that are separated 
    by newline characters, and executing each command as soon as 
    it is complete.
    */
    while((bytes=read(FD,buffer,BUFF_SIZE))>0){
        for(long int j=0; j<bytes;j++){
            if(buffer[j]=='\n'){
                buffer[j]='\0';
                //function call to parse the commands  
                //right here

                buffer[j]='\n';
            }
        }
        write(STDOUT_FILENO, buffer, bytes);

        //buffer command is the word exit
        if (strcmp(buffer, "exit\n") == 0){break;}
    }
        
    if (bytes == -1) {
    perror("Error reading file");
    exit(1);
    }

    close(FD);
}
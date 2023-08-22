#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
int main(int argc, char* argv[]){
    if (argc !=2){
        errx(EXIT_FAILURE, "Invalid number of parameters");
    }
    pid_t p1 = fork();
    if (p1 == -1){
        err(EXIT_FAILURE, "Invalid new thread");
    }
    int status;
    if (p1 > 0){
        wait (&status); 
        if (WIFEXITED(status) == 1){
            printf("The command is: %s\n",argv[1]);
        }
    }
    else if (p1 == 0){
        if(execlp(argv[1], argv[1], (char*)(NULL)) == -1){
            err(EXIT_FAILURE, "Command failed to execute");
        }
    }
    exit(0);
}

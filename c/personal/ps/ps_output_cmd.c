#include <unistd.h>
int main(void) {
    int fd[2];
    if (pipe(fd) < 0) {
        err(1, "could not create a pipe");
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        err(1, "could not fork");
    }

    int stat;
    if (pid == 0) {
        close(fd[0]);

        dup2(fd[1], STDOUT_FILENO);
    }
    else {
        
    }

    return 0;
}

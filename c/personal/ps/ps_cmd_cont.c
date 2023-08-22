#define _GNU_SOURCE
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    int fd[2];
    if (pipe(fd) < 0) {
        err(1, "could not pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        err(1, "could not fork");
    }

    int stat;
    if (pid == 0) {
        close(fd[0]);

        dup2(fd[1], 1);
        execlp(
               "find",
               "find", "/", "-maxdepth", "1", "-type", "d",
               (char*)NULL);
        err(1, "could not exec cmd");
    }
    else {
        close(fd[1]);

        char buf[32];
        int num_bytes;
        while ((num_bytes = read(fd[0], buf, sizeof(buf))) > 0) {
            if (write(1, buf, num_bytes) < 0) {
                warn("write failed");
            }
        }

        if (num_bytes < 0) {
            err(1, "could not read");
        }

        wait(&stat);
    }

    return 0;
}

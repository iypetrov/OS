#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <sys/types.h>

//cat /etc/passwd | head -n 25 | awk -F ':' '{print $1}'  | sort

void cat() {
    execlp(
        "cat",
        "cat", "/etc/passwd",
        (char*)NULL);
    err(1, "could not exec cat");
}

void head() {
    execlp(
        "head",
        "head", "-25",
        (char*)NULL);
    err(1, "could not exec head");
}

void awk() {
    execlp(
        "awk",
        "awk", "-F", "':'", "'{print $1}'",
        (char*)NULL);
    err(1, "could not exec awk");
}

void sort() {
    execlp(
        "sort",
        "sort",
        (char*)NULL);
    err(1, "could not exec sort");
}

int main() {
    int fd1[2];
    int fd2[2];

    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        err(1, "could not create a pipe");
    }

    pid_t pid1 = fork();
    pid_t pid2 = fork();

    if (pid1 < 0 || pid2 < 0) {
        err(1, "could not fork");
    }

    if (pid1 == 0 && pid2 == 0) {
        close(fd1[0]);
        dup2(fd1[1], 1);
        cat();
    }
    if (pid1 == 0 && pid2 != 0) {
        close(fd1[1]);
        dup2(fd1[0], 0);
        head();   
    }

    if (pid1 != 0 && pid2 == 0) {
        close(fd1[0]);
        dup2(fd1[1], 1);
        cat();
    }
    if (pid1 != 0 && pid2 != 0) {
        close(fd1[1]);
        dup2(fd1[0], 0);
        head();   
    }

    return 0;
}

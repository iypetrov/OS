#define _POSIX_C_SOURCE 200809L  // For strdup
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE_SIZE 1024
#define MAX_FILES 100

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) { // child process
        close(fd[0]);  // close the read end
        dup2(fd[1], STDOUT_FILENO); // make stdout write to the pipe
        close(fd[1]);
        execlp("find", "find", ".", "-name", "*.c", NULL);
        perror("execlp find failed");
        exit(1);
    }

    close(fd[1]); // close the write end in the parent

    char line[MAX_LINE_SIZE];
    char* files[MAX_FILES];
    int count = 0;

    while (count < MAX_FILES) {
        int bytes_read = read(fd[0], line, MAX_LINE_SIZE-1);
        if (bytes_read <= 0) break; // Check for EOF or error
        line[bytes_read] = '\0';  // Null terminate the string
        if (line[bytes_read-1] == '\n') line[bytes_read-1] = '\0'; // replace newline
        files[count] = strdup(line);
        count++;
    }

    close(fd[0]);

    for (int i = 0; i < count; i++) {
        if (fork() == 0) {  // child process
            printf("Process %d for file: %s\n", getpid(), files[i]);
            exit(0);
        }
    }

    // wait for all child processes to complete
    for (int i = 0; i < count; i++) {
        wait(NULL);
    }

    for (int i = 0; i < count; i++) {
        free(files[i]);
    }

    return 0;
}


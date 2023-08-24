#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

bool wait_child(void) {
	int stat;
	if (wait(&stat) < 0) {
		if (errno == ECHILD) {
			return false;
		}

		err(1, "can not wait");
	}
	if (WIFEXITED(stat) && !WEXITSTATUS(stat)) {
		return true;
	}
	err(1, "failed wait");
}

int main(void) {
	int fd[2];
	char buf[1024];

	if (pipe(fd) < 0) {
		err(1, "can not pipe");
	}

	pid_t pid = fork();
	if (pid < 0) {
		err(1, "can not fork");
	}

	if (pid == 0) {
		close(fd[0]);
		dup2(fd[1], 1);
		close(fd[1]);
		execlp("ls", "ls", (char*)NULL);
		err(1, "failed ls");
	}
	else {
		close(fd[1]);

		ssize_t len = read(fd[0], buf, sizeof(buf));
		if (len < 0) {
			err(1, "could not read");
		}

		buf[len] = '\0';
		printf("%s\n", buf);	

		wait_child();
	}

	return 0;
}

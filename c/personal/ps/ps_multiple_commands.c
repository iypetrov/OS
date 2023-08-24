#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
	int fdc;
	int fdv[1024];
} fd_ctx;

void init(fd_ctx* ctx) {
	ctx->fdc = 0;
}

void make_pipe(fd_ctx* ctx, int fd[2]) {
	if (pipe(fd) < 0) {
		err(1, "failed pipe");
	}

	if (ctx->fdc >= 1024) {
		err(1, "max fdv size");
	}

	ctx->fdv[ctx->fdc] = fd[1];
	ctx->fdv[ctx->fdc + 1] = fd[0];
	ctx->fdc += 2;
}

void close_range(fd_ctx* ctx, int start, int end) {
	if (start > end) {
		err(1, "start should be less than end");
	}

	for (int i = start; i <= end; i++) {
		if (ctx->fdv[i] != -1) {
			close(ctx->fdv[i]);
			ctx->fdv[i] = -1;
		}
	}
}

bool wait_child() {
	int status;
	if (wait(&status) < 0) {
		if (errno == ECHILD) {
			return false;		
		}
		err(1, "can not wait");
	}
	if (WIFEXITED(status) && !WEXITSTATUS(status)) {
		return true;
	}
	err(1, "failed wait");
}

// cat /etc/passwd | cut -d ':' -f 4,6 | sort -t ':' -k 1,1 -nr

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "provide 1 arg %s", argv[0]);
	}

	fd_ctx ctx;
	init(&ctx);

	int fd1[2];
	int fd2[2];
	int fd3[2];
	make_pipe(&ctx, fd1);
	make_pipe(&ctx, fd2);
	make_pipe(&ctx, fd3);

	pid_t pid1 = fork();
	if (pid1 < 0) {
		err(1, "could not fork");
	}
	if (pid1 == 0) {
		dup2(fd1[1], 1);
		close_range(&ctx, 0, ctx.fdc - 1);
		execlp("cat", "cat", "/etc/passwd", (char*)NULL);
		err(1, "failed cat");
	}

	pid_t pid2 = fork();
	if (pid2 < 0) {
		err(1, "could not fork");
	}
	if (pid2 == 0) {
		dup2(fd1[0], 0);
		dup2(fd2[1], 1);
		close_range(&ctx, 0, ctx.fdc - 1);
		execlp("cut", "cut", "-d", ":", "-f", "4,6", (char*)NULL);
		err(1, "failed cut");
	}

	pid_t pid3 = fork();
	if (pid3 < 0) {
		err(1, "could not fork");
	}
	if (pid3 == 0) {
		dup2(fd2[0], 0);
		dup2(fd3[1], 1);
		close_range(&ctx, 0, ctx.fdc - 1);
		execlp("awk", "awk", "-F", ":", "{print $1, $2}", (char*)NULL);
		err(1, "failed awk");
	}

	close_range(&ctx, 0, ctx.fdc - 2);
	for (int i = 0; i < ctx.fdc / 2; i++) {
		wait_child();
	}

	int fd_in = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd_in < 0) {
		err(1, "failed open a file");
	}

	dup2(fd3[0], 0);
	dup2(fd_in, 1);
	close(fd3[0]);
	close(fd_in);

	execlp("sort", "sort", "-t", ":", "-k", "1,1", "-nr", (char*)NULL);
	err(1, "failed sort");

	return 0;
}

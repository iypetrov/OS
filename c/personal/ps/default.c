#include<fcntl.h>
#include<err.h>
#include<errno.h>
#include<stdbool.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

typedef struct {
	int fdc;
	int fdv[1024];
} fd_ctx;

void ctx_init(fd_ctx* ctx) {
	ctx->fdc = 0;
}

void ctx_add(fd_ctx* ctx, int fd[2]) {
	if (pipe(fd) < 0) {
		err(1, "failed pipe");
	}

	if (ctx->fdc >= 1024) {
		err(1, "max size ctx");
	}

	ctx->fdv[ctx->fdc] = fd[0];
	ctx->fdv[ctx->fdc + 1] = fd[1];
	ctx->fdc += 2;
}

void ctx_close(fd_ctx* ctx) {
	for (int i = 0; i < ctx->fdc; i++) {
		close(ctx->fdv[i]);
	}
}

bool wait_child() {
	int stat;
	if (wait(&stat) < 0) {
		if (errno == ECHILD) {
			return false;
		}
		err(1, "could not wait");
	}

	if (WIFEXITED(stat) && !WEXITSTATUS(stat)) {
		return true;
	}
	err(1, "failed wait");
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "provide 1 arg %s", argv[0]);
	}

	fd_ctx ctx;
	ctx_init(&ctx);

	int fd1[2];
	ctx_add(&ctx, fd1);
	
	pid_t pid1 = fork();
	if (pid1<0)  {
		err(1, "failed can not fork");
	}

	if (pid1==0) {
		close(fd1[0]);
		dup2(fd1[1], 1);
		ctx_close(&ctx);
		execlp("find", "find", "/home", "-maxdepth", "2", "-type", "d", (char*)NULL);
		err(1, "error in find");
	}

	int fd2[2];
	ctx_add(&ctx, fd2);
	
	pid_t pid2 = fork();
	if (pid2<0)  {
		err(1, "failed can not fork");
	}

	if (pid2==0) {
		close(fd2[0]);
		dup2(fd1[0], 0);
		dup2(fd2[1], 1);
		ctx_close(&ctx);
		execlp("cut", "cut", "-d", "/", "-f", "4-5", (char*)NULL);
		err(1, "error in cut");
	}

	close(fd1[1]);
	close(fd1[0]);
	close(fd2[1]);

	wait_child();
	wait_child();

	int fd_in=open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_in < 0) {
		err(1, "could not open file for writing");
	}

	dup2(fd2[0], 0);
	dup2(fd_in, 1);
	close(fd_in);
	close(fd1[0]);

	execlp("sort", "sort", "-r", (char*)NULL);
	err(1, "error in sort");

	return 0;	
}

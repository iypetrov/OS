#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_NUM_FDS 1024

typedef struct {
    int num_fds;
    int fds[MAX_NUM_FDS];
} fd_ctx;

void init_ctx(fd_ctx* ctx) {
    ctx->num_fds=0;
}

void ctx_add(fd_ctx* ctx, int fd[2]) {
    if (pipe(fd) < 0) {
        err(1, "could not create pipe");
    }

    if (ctx->num_fds >= MAX_NUM_FDS) {
        err(1, "could not create more fds");
    }

    ctx->fds[ctx->num_fds] = fd[0];
    ctx->fds[ctx->num_fds + 1] = fd[1];
    ctx->num_fds += 2;
}

void close_pipes(fd_ctx* ctx) {
    for (int i = 0; i < MAX_NUM_FDS; i++) {
        close(ctx->fds[i]);
    }
}

bool wait_for_child(void) {
	int stat;
	
    if (wait(&stat) < 0) {
		if (errno == ECHILD) {
			return false;
		}
		err(1, "could not wait");
	}
	
    if (WIFEXITED(stat) && ! WEXITSTATUS(stat)) {
		return true;
	}
	
    err(1, "child failed");
}

int main(void) {
    return 0;
}

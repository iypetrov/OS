#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NUM_FDS 1024

typedef struct {
    int num_fds;
    int fds[MAX_NUM_FDS];
} fd_ctx;

void init_ctx(fd_ctx* ctx) {
    ctx->num_fds = 0;
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
    for (int i = 0; i < ctx->num_fds; i++) {
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

    if (WIFEXITED(stat) && !WEXITSTATUS(stat)) {
        return true;
    }

    err(1, "child failed");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        errx(1, "provide 1 arg and it should be a filename %s", argv[0]);
    }

    fd_ctx ctx;
    init_ctx(&ctx);

    int fd1[2];
    ctx_add(&ctx, fd1);
    if (fork() == 0) {
        dup2(fd1[1], 1);
        close_pipes(&ctx);
        execlp("cat", "cat", "/etc/passwd", NULL);
        err(1, "cat failed");
    }

    int fd2[2];
    ctx_add(&ctx, fd2);
    if (fork() == 0) {
        dup2(fd1[0], 0);
        dup2(fd2[1], 1);
        close_pipes(&ctx);
        execlp("head", "head", "-n", "25", NULL);
        err(1, "head failed");
    }

    int fd3[2];
    ctx_add(&ctx, fd3);
    if (fork() == 0) {
        dup2(fd2[0], 0);
        dup2(fd3[1], 1);
        close_pipes(&ctx);
        execlp("awk", "awk", "-F:", "{print $1}", NULL);
        err(1, "awk failed");
    }

    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    close(fd3[1]);

    while (wait_for_child());

    int fd_out = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd_out < 0) {
        err(1, "could not open file for writing");
    }

    dup2(fd3[0], 0);
    dup2(fd_out, 1);
    close(fd_out);
    close(fd3[0]);

    execlp("sort", "sort", "-r", NULL);
    err(1, "sort failed");

    return 0;
}



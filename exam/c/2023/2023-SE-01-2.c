// Напишете програма на C, която приема като аргумент име на директория и генерира хешове на
// файловете в нея, работейки паралелно.
// При изпълнение върху директория my-dir, програмата:
// • За всеки файл в директорията и нейните поддиректории (напр. my-dir/foo/my-file), трябва
// да създаде файл my-dir/foo/my-file.hash, съдържащ хеш-сумата на my-file
// • Може да извиква външните команди find (за откриване на файлове) и md5sum (за пресмятане
// на хеш-сума)
// • Не трябва да обработва файлове, чиито имена вече завършват на .hash
// • Трябва да позволява md5sum-процесите да работят паралелно един спрямо друг


// find . -type f ! -name "*.hash"
// md5sum file.hash | awk '{print $1}'
// find . -type f ! -name "*.hash" -print0 | xargs -0 -n1 md5sum

#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

typedef struct {
	int fdc;
	int fdv[1024];
} fd_ctx;

void init_ctx(fd_ctx* ctx) {
	ctx->fdc = 0;
}

void make_pipe(fd_ctx* ctx, int fd[2]) {
	if (pipe(fd) < 0) {
		err(1, "failed pipe");
	}

	if (ctx->fdc > 1024) {
		err(1, "max size");
	}

	ctx->fdv[ctx->fdc] = fd[1];
	ctx->fdv[ctx->fdc + 1] = fd[0];
	ctx->fdc += 2;
}

void close_all(fd_ctx* ctx, size_t start, size_t end) {
	for (size_t i = start; i <= end; i++) {
		close(ctx->fdv[i]);
	}
	ctx->fdc = 0;
}

bool wait_child() {
	int stat;
	if (wait(&stat) < 0) {
		if (errno != ECHILD) {
			return false;
		}
		err(1, "can not wait");
	}

	if (WIFEXITED(stat) && !WEXITSTATUS(stat)) {
		return true;
	}

	err(1, "failed to wait");
}

bool check_is_dir(char* dirname) {
	struct stat metadata;
	if (stat(dirname, &metadata) < 0) {
		errx(1, "failed to load metadata");
	}

	return S_ISDIR(metadata.st_mode); 
}

void spawn_file_walker(fd_ctx* ctx, int fd_out, char* src) {
	int fd[2];
	make_pipe(ctx, fd);

	pid_t pid = fork();
	if (pid < 0) {
		err(1, "failed fork");
	}
	if (pid == 0 ) {
		if (dup2(fd[1], fd_out) < 0) {
			err(1, "failed dup2");
		}
		close_all(ctx, 0, 1023);

		execlp("find", "find", src, "-type", "f", "!", "-name" "*hash", (char*)NULL);
		err(1, "failed find");
	}
}

bool read_filename() {
	

	return true;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		err(1, "provide 1 arg %s", argv[0]);
	}

	if (!check_is_dir(argv[1])) {
		err(1, "provide dir");
	}

	fd_ctx ctx;
	init_ctx(&ctx);

	int fd_main[2];
	make_pipe(&ctx, fd_main);

	spawn_file_walker(&ctx, fd_main[1], argv[1]);
	close(fd_main[1]);
	
	//wait_child();
	

	while(wait_child());
	close_all(&ctx, 0, 1023);

	return 0;
}

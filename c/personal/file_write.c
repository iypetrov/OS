#include <err.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
	int fd = open(
            "file.txt",
            O_WRONLY | O_CREAT | O_TRUNC,
            0777);

    if (fd < 0) {
        err(1, "could not open a fd");
    }

    char buff[] = "hello\n";
    if ((write(fd, buff, sizeof(buff))) < 0) {
        err(1, "coluld not write to a file");
    }

    if (close(fd) < 0) {
        err(1, "coluld not close a fd");
    }

    return 0;
}

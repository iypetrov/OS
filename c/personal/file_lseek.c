#include <err.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(void) {
    int fd = open(
            "file.txt",
            O_RDONLY);

    if (fd < 0) {
        err(1, "could not open a file");
    }

    dprintf(1, "the size of file is %ld\n", lseek(fd, 0, SEEK_END));
    dprintf(1, "the current pos if cursor of file is %ld\n", lseek(fd, 0, SEEK_CUR));

    close(fd);

	return 0;
}

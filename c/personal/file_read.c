#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
	int fd = open(
            "file.txt",
            O_RDONLY);

    if (fd < 0) {
        err(1, "could not open the file");
    }

    char buff[512];
    int num_bytes;
    while ((num_bytes = read(fd, buff, sizeof(buff))) > 0) {
        dprintf(1, "%s", buff);
    }

    if (num_bytes < 0) {
        dprintf(1, "could not read the file");
    }

    if (close(fd) < 0) {
        err(1, "could not close the file");
    }

    return 0;
}

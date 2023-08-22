#include <err.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

typedef struct {
    int id;
    char name[1024];
} person;

int main(void) {
    char filename[] = "data.bin";
    int fd_in = open(
            filename,
            O_WRONLY | O_CREAT | O_TRUNC,
            0644);
   
    int fd_out = open(
            filename,
            O_RDONLY);

    if (fd_in < 0) {
        err(1, "could not open the file for writing");
    }

    if (fd_out < 0) {
        err(1, "could not open the file for reading");
    }

    //BI

    //write
    person p_in;
    p_in.id=1;
    char name[1024] = "Ivan";
    strcpy(p_in.name,name);

    if ((write(fd_in, &p_in, sizeof(person))) < 0) {
        err(1, "could not write the person obj to a bin file");   
    }

    //read
    person p_out;
    int num_bytes;
    while ((num_bytes = read(fd_out, &p_out, sizeof(person))) > 0) {
        dprintf(1,"%d\n", p_out.id);
        dprintf(1,"%s\n", p_out.name);
    }

    if (num_bytes < 0) {
        err(1, "error while read the file");
    }

    if (close(fd_in) < 0) {
        err(1, "could not close the file for writing");
    }

    if (close(fd_out) < 0) {
        err(1, "could not close the file for reading");
    }

    return 0;
}

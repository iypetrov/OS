// Напишете програма на C, която да работи подобно на командата cat, реализирайки
// само следната функционалност:
// • общ вид на изпълнение: ./main [OPTION] [FILE]...
// • ако е подаден като първи параметър -n, то той да се третира като опция, което кара програ-
// мата ви да номерира (глобално) всеки изходен ред (започвайки от 1).
// • програмата извежда на STDOUT
// • ако няма подадени параметри (имена на файлове), програмата чете от STDIN
// • ако има подадени параметри – файлове, програмата последователно ги извежда
// • ако някой от параметрите е тире (-), програмата да го третира като специално име за STDIN
// Примерно извикване:
// $ cat a.txt
// a1
// a2
// $ cat b.txt
// b1
// b2
// b3
// $ echo -e "s1\ns2" | ./main -n a.txt - b.txt
// 1 a1
// 2 a2
// 3 s1
// 4 s2
// 5 b1
// 6 b2
// 7 b3
#include <err.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

char* add_numbers(const char* str) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            count++;
        }
    }

    char* new_str = malloc(strlen(str) + count + 1);
    if (!new_str) {
		err(1, "fck");
	}

    int j = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            new_str[j++] = '\n';
            new_str[j++] = '4';
        } else {
            new_str[j++] = str[i];
        }
    }

    new_str[j] = '\0'; 
    return new_str;
}

int main (int argc, char* argv[]) {
	int num_files = argc;
	int start = 1;

	if (!strcmp(argv[1], "-n")) {
		num_files = argc - 1;
		start = 2;
	}

	for (int i = start; i < num_files; i++) 
	{
		int fd_in;
		if (!strcmp(argv[i], "-")) {
			fd_in = 0;
		}
		else {
			fd_in = open(argv[i], O_RDONLY);
			if (fd_in < 0) {
				err(1, "error open the file");
			}
		}

		int num_bytes;
		char buf [1024];
		while ((num_bytes = read(fd_in, buf, sizeof(buf))) != 0) {
			if (num_bytes < 0) {
				err(1, "error readign the file");
			}
			char* d = add_numbers(buf);
			if (write(1, d, sizeof(&d)) < 0) {
				err(1, "could not write the input");
			}
		}

		if (close(fd_in) < 0) {
			err(1, "error close the file");
		}
	}

	return 0;
}

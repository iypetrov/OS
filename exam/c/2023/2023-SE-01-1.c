// Имате файл, съдържащ откъс от поток от съобщения, който може би е повреден на места.
// Трябва да напишете програма, която по даден такъв файл, генерира нов файл, съдържащ точно
// всички последователности от байтове от оригиналния, които са валидни съобщения.
// Примерно извикване на програмата: ./main stream.bin messages.bin, където stream.bin е име
// на съществуващ файл, а messages.bin трябва да се създаде (или презапише).
// Валидно съобщение с дължина N байта има вида:
// • байт 1 — 0x55 – указва начало на съобщение
// • байт 2 — числото N – представено като 8-битово число без знак
// • байтове от 3 до (N − 1) — произволни данни (съдържанието на съобщението)
// • байт N — checksum на байтове от 1 до (N − 1)
// Checksum на поредица от байтове е резултатът от прилагане операцията “побитово изключващо
// или“ (xor) върху тях. Възползвайте се от него, за да проверявате валидността на съобщенията
#include <stdbool.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

bool is_reg_file(const char* path) {
	struct stat md;
	if (stat(path, &md) < 0) {
		return false;
	}

	return S_ISREG(md.st_mode);
}

off_t seek(int fd, off_t offset, int whence) {
	off_t res = lseek(fd, offset, whence);
	if (res < 0) {
		err(1, "error lseek");
	}

	return res;
}

bool read_byte(int fd, uint8_t* buf) {
	int res;
	if ((res = read(fd, buf, sizeof(buf))) < 0) {
		err(1, "failed read byte");
	}
	
	if (res == 0) {
		return false;
	}

	return true;
}

void clear(int* stage, uint8_t* start, uint8_t* N, uint8_t (*msg)[1024]) {
	memset(&stage, 0, sizeof(int));
	memset(&start, 0, sizeof(uint8_t));
	memset(&N, 0, sizeof(uint8_t));
	memset(&msg, 0, 1024 * sizeof(uint8_t));
}

void handle(int fd_in, int fd_out) {
	off_t file_size = seek(fd_in, 0, SEEK_END);

	// 1 - start
	// 2 - define N
	// 3 - read msg
	// 4 - checksum
	int stage = 1;
	for (uint8_t i = 0; i < file_size - 3; i++) {
		if (seek(fd_in, i, SEEK_SET) < 0) {
			err(1, "err iterating src");
		}

		uint8_t buf;
		uint8_t start;
		uint8_t N;
		uint8_t msg[1024];
		if (!read_byte(fd_in, &buf)) {
			return;
		}

		// BI
		if (buf != 0x55 && stage == 0) {
			continue;
		}

		if (buf == 0x55 && stage != 0) {
			clear(&stage, &start, &N, &msg);
			continue;
		}

		// Stages
		if (stage == 1) {
			if (buf == 0x55) {
				stage++;
			}
		}
		else if (stage == 2) {
			if (buf < 3) {
				clear(&stage, &start, &N, &msg);
			}

			N = buf;
			start = i;
			stage++;
		}	
		else if (stage == 3) {
			if (i > (start + N - 3)) {
				stage++;
			}
			
			msg[i - start - 1] = buf;
		}
		else if (stage == 4) {
			uint8_t checksum = start ^ N;
			for (uint8_t i = 0; i < N - 3; i++) {
				checksum ^= msg[i];
			}

			if (buf == checksum) {
				uint8_t tmp = 0x55;
				if (write(fd_out, &tmp, sizeof(uint8_t)) < 0) {
					err(1, "could not write start");
				}
				if (write(fd_out, &N, sizeof(uint8_t)) < 0) {
					err(1, "could not write N");
				}
				if (write(fd_out, &msg, N - 3) < 0) {
					err(1, "could not msg");
				}
				if (write(fd_out, &checksum, sizeof(uint8_t)) < 0) {
					err(1, "could not write checksum");
				}
			}
			clear(&stage, &start, &N, &msg);
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		err(1, "wrong num of args %s", argv[0]);
	}

	if (!is_reg_file(argv[1])) {
		err(1, "not valid filename");
	}

	int fd_in = open(argv[1], O_RDONLY);
	if (fd_in < 0) {
		err(1, "error open file reading");
	}
	
	int fd_out = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd_out < 0) {
		err(1, "error open file writing");
	}

	handle(fd_in, fd_out);

	if (close(fd_in) < 0) {
		err(1, "error close file reading");
	}

	if (close(fd_out) < 0) {
		err(1, "error close file writing");
	}

	return 0;
}

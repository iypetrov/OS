// Инженерите от съседната лабораторя ползват специализиран хардуер и софтуер за
// прехвърляне на данни по радио, но за съжаление имат два проблема:
// • в радио частта: дълги поредици битове само 0 или само 1 чупят преноса;
// • в софтуерната част: софтуерът, който ползват, може да прехвърля само файлове с четен брой
// байтове дължина.
// Помогнете на колегите си, като напишете програма на C, която решава тези проблеми, като подготвя
// файлове за прехвърляне. Програмата трябва да приема два задължителни позиционни аргумента
// – имена на файлове. Примерно извикване:
// $ ./main input.bin output.bin
// Програмата чете данни от input.bin и записва резултат след обработка в output.bin. Програмата
// трябва да работи като encoder, който имплементира вариант на Manchester code, т.е.:
// • за всеки входен бит 1 извежда битовете 10, и
// • за всеки входен бит 0 извежда битовете 01
// Например, следните 8 бита вход
// 1011 0110 == 0xB6
// по описаният алгоритъм дават следните 16 бита изход
// 1001 1010 0110 1001 == 0x9A69

#define _GNU_SOURCE

#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

uint16_t encode(const uint8_t number) {
    uint16_t result = 0;
    
    for (int i = 0; i < 8; i++) {
        if (number & (1 << i)) {
            result |= (1 << (i * 2 + 1));
        } else {
            result |= (1 << (i * 2));
        }
    }

    return result;
}

void handle(int fd_in, int fd_out) {
	int num_bytes_in;
	uint8_t buf;
	while ((num_bytes_in = read(fd_in, &buf, sizeof(uint8_t))) > 0) {
		uint16_t res = encode(buf);
		if ((write(fd_out, &res, sizeof(uint16_t))) < 0) {
			err(1, "can not write");
		}
	}

	if (num_bytes_in < 0) {
		err(1, "can not read");
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		errx(1, "provide 2 args %s", argv[0]);
	}

	int fd_in = open(argv[1], O_RDONLY);
	if (fd_in < 0) {
		err(1, "can not open file for reading");
	}

	int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out < 0) {
		err(1, "can not open file for writing");
	}

	handle(fd_in, fd_out);

	if (close(fd_in) < 0) {
		err(1, "can not close read fd");
	}

	if (close(fd_out) < 0) {
		err(1, "can not close write fd");
	}

	return 0;
}


// Напишете програма на C, която да работи подобно на командата tr, реализирайки
// само следната функционалност:
// • програмата чете от стандартния вход и пише на стандартния изход
// • общ вид на изпълнение: ./main [OPTION] SET1 [SET2]
// • OPTION би могъл да бъде или -d, или -s, или да липсва
// • SET1 и SET2 са низове, в които знаците нямат специално значение, т.е. всеки знак “означава”
// съответния знак. SET2, ако е необходим, трябва да е същата дължина като SET1
// • ако е подаден като първи параметър -d, програмата трие от входа всяко срещане на знак μ
// от SET1; SET2 не е необходим
// • ако е подаден като първи параметър -s, програмата заменя от входа всяка поредица от повто-
// рения знак μ от SET1 с еднократен знак μ; SET2 не е необходим
// • в останалите случаи програмата заменя от входа всеки знак μ от SET1 със съответстващият
// му позиционно знак ν от SET2.
// Примерно извикване:
// $ echo asdf | ./main -d ’d123’ | ./main ’sm’ ’fa’ | ./main -s ’f'
// af
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#include <string.h>
#include <stdbool.h>

bool equals(char* a, char* b) {
	if (strlen(a) != strlen(b)) {
		return false;
	}

	size_t len = strlen(a); 
	for (size_t i = 0; i < len; i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}

	return true;
}

bool wait_child() {
	int stat;
	if (wait(&stat) < 0) {
		if (errno == ECHILD) {
			return false;
		}
		err(1, "can not wait");
	}
	if (WIFEXITED(stat) && !WEXITSTATUS(stat)) {
		return true;
	}
	err(1, "failed to wait");
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		errx(1, "provide 2 args %s", argv[0]);
	}

	if (!equals("-s", argv[1]) && !equals("-d", argv[1]) && (strlen(argv[1]) != strlen(argv[2]))) {
		err(1, "wrong args");
	}

	if (equals("-s", argv[1])) {
		execlp("tr", "tr", "-s", argv[2], (char*)NULL);
		err(1, "failed -s");
	}

	if (equals("-d", argv[1])) {
		execlp("tr", "tr", "-d", argv[2], (char*)NULL);
		err(1, "failed -d");
	}

	execlp("tr", "tr", argv[1], argv[2], (char*)NULL);
	err(1, "failed -d");
	
	return 0;
}

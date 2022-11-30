#include "syscall.h"

int main() {
	int c;
	c = CreateSemaphore("test", 1);
	PrintString("Success");
	return 0;
}

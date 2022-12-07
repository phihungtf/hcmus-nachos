#include "syscall.h"

int main() {
	OpenFileId id;
	char buffer[15];
	Create("test2.txt");
	id = Open("test2.txt", 0);
	Write("Hello, World!@#", 15, id);
	Close(id);
	id = Open("test2.txt", 1);
	Read(buffer, 15, id);
	Close(id);
	PrintString(buffer);
	PrintString("\n");
	return 0;
}
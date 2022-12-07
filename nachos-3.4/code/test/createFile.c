#include "syscall.h"

int main() {
	OpenFileId id;
	char buffer[13];
	Create("test.txt");
	id = Open("test.txt", 0);
	Write("Hello, World!", 13, id);
	Close(id);
	id = Open("test.txt", 1);
	Read(buffer, 13, id);
	Close(id);
	PrintString(buffer);
	PrintString("\n");
	return 0;
}
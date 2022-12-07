#include "syscall.h"

int main() {
	OpenFileId id;
	char buffer[12];
	Create("test.txt");
	id = Open("test.txt", 0);
	Write("Hello, World", 12, id);
	Close(id);
	id = Open("test.txt", 1);
	Read(buffer, 12, id);
	Close(id);
	PrintString(buffer);
	PrintString("\n");
	return 0;
}
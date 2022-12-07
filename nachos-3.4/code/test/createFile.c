#include "syscall.h"

int main() {
	OpenFileId id;
	char buffer[11];
	Create("test.txt");
	id = Open("test.txt", 0);
	Write("Hello World", 11, id);
	Close(id);
	id = Open("test.txt", 1);
	Read(buffer, 11, id);
	Close(id);
	PrintString(buffer);
	return 0;
}
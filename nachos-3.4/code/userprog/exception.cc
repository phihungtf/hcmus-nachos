// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"

#define MAX_FILE 10

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void fetchNext()
{
	// Tang thanh ghi program counter
	machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
	machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
	machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}

void extraCharacterRecycle()
{
	char *recycle = new char[1001];
	synchconsole->Read(recycle, 1000);
	delete[] recycle;
}

char ReadChar()
{
	char c;
	int numBytesRead = synchconsole->Read(&c, 1);
	if (numBytesRead < 0)
	{
		printf("\nStop.\n");
	}
	extraCharacterRecycle();
	return c;
}

int ReadInt()
{
	char *str = new char[101];
	int temp = synchconsole->Read(str, 100);
	if (temp < 0)
	{
		printf("Stop");
		delete[] str;
		return 0;
	}
	int flag = 0;
	if (str[0] == '-')
		flag = 1;
	int result = 0;
	int count = 0;
	for (int i = flag; i < 100; i++)
		if (str[i] <= '9' && str[i] >= '0')
			count++;
	for (int i = flag; i < count + flag; i++)
		if (str[i] <= '9' && str[i] >= '0')
		{
			int pow = 1;
			for (int j = i; j < count + flag - 1; j++)
				pow = pow * 10;
			result += (str[i] - '0') * pow;
		}
	if (flag)
		result = -result;
	delete[] str;
	return result;
}

void PrintChar(char character)
{
	if (character < 0 || character > 255)
	{
		printf("\n Not ASCII code: %d\n", character);
		return;
	}
	synchconsole->Write(&character, 1);
}

void PrintInt(int number)
{
	int flag = 0;
	int temp = number;
	if (number < 0)
	{
		flag = 1;
		temp = -temp;
	}
	int temp1 = temp;
	int count = 1;
	while (temp / 10 != 0)
	{
		count++;
		temp /= 10;
	}
	if (count > 0)
	{
		char *str = new char[count + flag];
		if (flag)
			str[0] = '-';
		for (int i = flag; i < count + flag; i++)
		{
			int pow = 1;
			for (int j = i; j < count + flag - 1; j++)
				pow *= 10;
			str[i] = temp1 / pow + '0';
			temp1 = temp1 - (temp1 / pow * pow);
		}
		synchconsole->Write(str, count + flag);
	}
}

char *User2System(int virtAddr, int limit)
{
	int i;
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1];
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void ReadString(char *buffer, int length)
{
	int addrBuf = machine->ReadRegister(4);
	buffer = User2System(addrBuf, length);
	synchconsole->Read(buffer, length);
	System2User(addrBuf, length, buffer);
	extraCharacterRecycle();
}

void PrintString(char *buffer)
{
	int addrBuf = machine->ReadRegister(4);
	buffer = User2System(addrBuf, 1000);
	int lenBuf = 0;
	for (int i = 0; buffer[i] != 0; i++)
		lenBuf++;
	synchconsole->Write(buffer, lenBuf);
}

// Check whether can create file or not
int Create(char *fileName) // Check if can create file or not
{
	int fileName_length = strlen(fileName);
	if (fileName_length == NULL)
	{
		DEBUG('f', "\nNot enough memory in system");
		return 0;
	}
	else if (fileName_length == 0)
	{
		DEBUG('f', "\nFile name can not be empty");
		return 0;
	}
	else
	{
		DEBUG('f', "\n Finish reading filename.");
		if (!fileSystem->Create(fileName))
		{
			DEBUG('f', "\nError create file");
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

// hamdle for SC_CreateFile
void CreateFile()
{
	int virtAddr;
	char *file_name;
	virtAddr = machine->ReadRegister(4);
	file_name = User2System(virtAddr, 32); // 32 is max file length
	if (Create(file_name))
	{
		machine->WriteRegister(2, 0);
	}
	else
	{
		machine->WriteRegister(2, -1);
	}
	delete[] file_name;
}

// handle for SC_OpenFile
void OpenFile()
{
	char *file_name;									   // file name
	int id;												   // id of file
	file_name = User2System(machine->ReadRegister(4), 32); // convert from user's buffer to kernel's buffer
	id = (int)fileSystem->openFile(file_name);			   // get id of that file
	machine->WriteRegister(2, id);
	delete[] file_name;
}

// handle for SC_CloseFile
void CloseFile()
{
	int id;
	id = machine->ReadRegister(4); // read id of file
	machine->WriteRegister(2, fileSystem->Close(id));
}

// handle for SC_ReadFile
void ReadFile()
{
	int virtAddr = machine->ReadRegister(4);
	int numBytes = machine->ReadRegister(5);
	int id = machine->ReadRegister(6);
	char *buffer = new char[numBytes];
	if (id == 0) // if using console input
	{
		machine->WriteRegister(2, numBytes);
		System2User(virtAddr, numBytes, buffer);
	}
	else if (id < 0 || id >= MAX_FILE || id == 1) // if id is invalid, return -1
	{
		machine->WriteRegister(2, -1);
	}
	else // if read data from file
	{
		machine->WriteRegister(2, (int)fileSystem->Read(buffer, numBytes, id));
		System2User(virtAddr, numBytes, buffer);
	}
	delete[] buffer;
	buffer = NULL;
}

// handle for SC_WriteFile
void WriteFile()
{
	int virtAddr = machine->ReadRegister(4);
	int numBytes = machine->ReadRegister(5);
	int id = machine->ReadRegister(6);
	char *buffer = User2System(virtAddr, 32);
	if (id == 1) // if using console output
	{
		machine->WriteRegister(2, numBytes);
	}
	else if (id < 0 || id >= MAX_FILE || id == 0)
	{
		machine->WriteRegister(2, -1);
	}
	// if write data to file
	else
		machine->WriteRegister(2, (int)fileSystem->Write(buffer, numBytes, id));
	delete[] buffer;
	buffer = NULL;
}

SpaceId Exec(char *name)
{
	if (name == NULL)
	{
		printf("file doesn't open");
		fflush(stdin);
		return -1;
	}
	OpenFile *file = fileSystem->Open(name);
	if (file == NULL)
	{
		printf("file doesn't open");
		fflush(stdin);
		return -1;
	}

	delete file;
	SpaceId pID = processTab->ExecUpdate(name);

	return pID;
}

int Join(SpaceId id)
{
	int res;
	res = processTab->JoinUpdate(id);
	return res;
}

void Exits(int status)
{
	if (status != 0)
	{
		fetchNext();
		return;
	}
	int res;
	res = processTab->ExitUpdate(status);
	currentThread->FreeSpace();
	currentThread->Finish();
	fetchNext();
	return;
}

int CreateSemaphore(char *name, int semval)
{
	if (name == NULL)
	{
		DEBUG('a', "Error: not enough memory");
		machine->WriteRegister(2, -1);
		delete[] name;
		return -1;
	}
	int result = semTab->Create(name, semval);
	if (result == -1)
	{
		DEBUG('a', "Can not create semaphore");
		machine->WriteRegister(2, result);
		delete[] name;
		return -1;
	}
	machine->WriteRegister(2, result);
	delete[] name;
	return 0;
}

int Wait(char *name)
{
	if (name == NULL)
	{
		DEBUG('a', "Error: not enough memory");
		machine->WriteRegister(2, -1);
		delete[] name;
		return -1;
	}
	int result = semTab->Wait(name);
	if (result == -1)
	{
		DEBUG('a', "Error: Semaphore is not found");
		machine->WriteRegister(2, result);
		delete[] name;
		return -1;
	}
	machine->WriteRegister(2, result);
	delete[] name;
	return 0;
}
int Signal(char *name)
{
	if (name == NULL)
	{
		DEBUG('a', "Error: not enough memory");
		machine->WriteRegister(2, -1);
		delete[] name;
		return -1;
	}
	int result = semTab->Signal(name);
	if (result == -1)
	{
		DEBUG('a', "Error: Semaphore is not found");
		machine->WriteRegister(2, result);
		delete[] name;
		return -1;
	}
	machine->WriteRegister(2, result);
	delete[] name;
	return 0;
}
void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	int virtadd, semval;
	char *name;
	int res;
	if (which == SyscallException)
	{
		switch (type)
		{
		// Khi nguoi dung goi syscall halt
		case SC_Halt:
			DEBUG('a', "Shutdown, initiated by user program.\n");
			interrupt->Halt();
			break;
		case SC_Exec:
			int addrN;
			addrN = machine->ReadRegister(4);
			name = User2System(addrN, 100);
			SpaceId k;
			k = Exec(name);
			machine->WriteRegister(2, k);
			break;
		case SC_Join:
			int id, res;
			id = machine->ReadRegister(4);
			res = Join(id);
			machine->WriteRegister(2, res);
			break;
		case SC_Exit:
			int statusE;
			statusE = machine->ReadRegister(4);
			Exits(statusE);
			break;
		case SC_ReadChar:
			char e;
			e = ReadChar();
			machine->WriteRegister(2, e);
			break;
		case SC_PrintChar:
			char d;
			d = machine->ReadRegister(4);
			PrintChar(d);
			break;
		case SC_ReadInt:
			int t;
			t = ReadInt();
			machine->WriteRegister(2, t);
			break;
		case SC_PrintInt:
			int m;
			m = machine->ReadRegister(4);
			PrintInt(m);
			break;
		case SC_ReadString:
			int length;
			length = machine->ReadRegister(5);
			char *buffer;
			ReadString(buffer, length);
			break;
		case SC_PrintString:
			char *buf;
			PrintString(buf);
			break;
		case SC_CreateSemaphore:
			virtadd = machine->ReadRegister(4);
			semval = machine->ReadRegister(5);
			name = User2System(virtadd, semval);
			res = CreateSemaphore(name, semval);
			break;
		case SC_Wait:
			virtadd = machine->ReadRegister(4);
			name = User2System(virtadd, -1);
			Wait(name);
			break;
		case SC_Signal:
			virtadd = machine->ReadRegister(4);
			name = User2System(virtadd, -1);
			Signal(name);
			break;
		}
		fetchNext();
	}
	else
	{
		printf("Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
	}
}

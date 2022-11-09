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
 
// Tang Program Counter
void incProgCounter() {
	machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
	machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
	machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}
 
void handleRuntimeError(const char msg[]) {
		DEBUG('a', (char *)msg);
		printf("%s", msg);
		interrupt->Halt();
}


//ham luu tru ki tu thua.
void extraCharacterRecycle(){
	char* recycle=new char[1001];
	synchconsole->Read(recycle,1000);
	delete[] recycle;
}

//Ham doc 1 ki tu.
char ReadChar() {
	char c;
	int numBytesRead = synchconsole->Read(&c, 1);//doc ki tu.
	//Kiem tra nguoi nhap co huy qua trinh nhap.
	if (numBytesRead < 0) {
		printf("\nStop.\n");
	}
	//Luu ki tu thua.
	extraCharacterRecycle();
	return c;
}

//Ham doc mot so.
int ReadInt(){
	char*str=new char[101];

	int temp = synchconsole->Read(str,100); // doc chuoi str chua cac ki tu bieu dien so.
	//Kiem tra nguoi dung co huy qua trinh.
	if(temp<0){
		printf("Stop");
		delete[]str;
		return 0;
	}
	//Kiem tra so am hoac duong
	int flag = 0;
	if(str[0]=='-')	flag = 1;
	int result = 0;
	//Dem so luong ki so
	int count=0;
	for(int i=flag; i<100;i++)
		if(str[i]<='9'&&str[i]>='0') count++;
	//chuyen tu chuoi ki so sang so
	for(int i = flag; i<count+flag; i++)
		if(str[i]<='9'&&str[i]>='0'){
			int pow = 1;
			for(int j=i;j<count+flag-1;j++)	pow = pow*10;
			result += (str[i]-'0')*pow;
		}
	//Doi dau ket qua neu so am
	if(flag) {result=-result;}
	delete[]str;
	return result;
}


//Ham in ra ki tu
//input: character(char)
void PrintChar(char character) {
	//kiem tra co phai ki tu ma ASCII
	if (character < 0 || character > 255) {
		printf("\n Not ASCII code: %d\n", character);
		return;
	}
	//in ra
	synchconsole->Write(&character, 1);
	//synchconsole->Write("\n", 1);
}

//Ham in so ra man hinh
//input: number
void PrintInt(int number){
	int flag = 0,temp,temp1;
	temp = number;
	//Kiem tra so am hoac duong
	if(number<0){
		flag = 1;
		temp = -1*number;
	}
	temp1 = temp;
	//printf("%d\n",temp1);
	//Dem so luong ki chu so
	int count = 0;
	while(temp>0){
		count++;
		temp/=10;
	}
	if(number==0)count=1;
	//phan tach so va dua vao chuoi
	if(count>0){
		char*str=new char[count+flag+1];
		if(flag) str[0]='-';
		for(int i=flag; i<count+flag;i++){
			int pow=1;
			for(int j=i;j<count+flag-1;j++) pow*=10;
			str[i]=temp1/pow+'0';
			temp1 = temp1-(temp1/pow)*pow;
		}
		//in ra man hinh
		synchconsole->Write(str,count+flag);
		delete []str;
	}
}

//Ham copy tu user sang system memory space tai vi tri virtAddr voi kich thuoc limit
char* User2System(int virtAddr,int limit){
	int i;
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf=new char[limit+1];
	if(kernelBuf==NULL) return kernelBuf;
	memset(kernelBuf,0,limit+1);
	for(i=0; i<limit;i++){
		machine->ReadMem(virtAddr+i, 1, &oneChar);
		kernelBuf[i]=(char)oneChar;
		if(oneChar==0) break;
	}
	return kernelBuf;
}

//Ham copy buffer tu system sang user memory space
int System2User(int virtAddr, int len, char* buffer){
	if(len<0) return -1;
	if(len==0) return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar=(int)buffer[i];
		machine->WriteMem(virtAddr+i,1,oneChar);
		i++;
	}while(i < len && oneChar != 0);
	return i;
}

//Ham doc chuoi. 
//input: chuoi buffer(char*), kich thuoc chuoi length.
void ReadString(char* buffer,int length){
	int addrBuf = machine->ReadRegister(4); //doc dia chi cua buffer.
	buffer = new char[length+1];
	int temp = synchconsole->Read(buffer,length);//doc chuoi vao.
	//kiem tra nguoi nhap co huy qua trinh nhap khong.
	if(temp<0){
		printf("stop\n");
		delete[]buffer;
		return;	
	}
	System2User(addrBuf,length,buffer); //copy buffer tu system sang user memory space.
	extraCharacterRecycle();//Luu tru ki tu thua neu so luong ki tu nhap vuot qua length.
}

void PrintString(char* buffer){
	int addrBuf = machine->ReadRegister(4);//doc dia chi buffer.
	buffer = User2System(addrBuf, 1000); //copy buffer tu user sang system memory space.
	int lenBuf = 0;
	//dem kich thuoc buffer
	for(int i=0; buffer[i]!= 0;i++)	lenBuf++;
	//in ra
	synchconsole->Write(buffer,lenBuf);
	//synchconsole->Write("\n",1);
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
 
	switch (which) {
		case NoException:
			return;
		case PageFaultException:
			handleRuntimeError("\n PageFaultException: No valid translation found.");
			break;
 
		case ReadOnlyException:
			handleRuntimeError("\n ReadOnlyException: Write attempted to page marked \"read-only\".");
			break;
 
		case BusErrorException:
			handleRuntimeError("\n BusErrorException: Translation resulted invalid physical address.");
			break;
 
		case AddressErrorException:
			handleRuntimeError("\n AddressErrorException: Unaligned reference or one that was beyond the end of the address space.");
			break;
 
		case OverflowException:
			handleRuntimeError("\n OverflowException: Integer overflow in add or sub.");
			break;
 
		case IllegalInstrException:
			handleRuntimeError("\n IllegalInstrException: Unimplemented or reserved instr.");
			break;
 
		case NumExceptionTypes:
			handleRuntimeError("\n NumExceptionTypes: Number exception types.");
			break;
 
		case SyscallException:
			switch (type) {
				case SC_Halt:
					DEBUG('a', "Shutdown, initiated by user program.\n");
					printf("Shutdown, initiated by user program.\n");
				   	interrupt->Halt();
				   	break;
				// Khi nguoi dung goi syscall ReadChar.
				case SC_ReadChar:            	
					char e;
					e = ReadChar();
					machine->WriteRegister(2,e);
					break;	
				// Khi nguoi dung goi syscall PrintChar.
				case SC_PrintChar:
					char d;
					d = machine->ReadRegister(4);
					PrintChar(d);
					break;
				// Khi nguoi dung goi syscall ReadInt.
				case SC_ReadInt:            	
					int g;
					g = ReadInt();
					machine->WriteRegister(2,g);
					break;
				// Khi nguoi dung goi syscall PrintInt.
				case SC_PrintInt:
					int m;
					m = machine->ReadRegister(4);
					PrintInt(m);
					break;
				// Khi nguoi dung goi syscall ReadString.
				case SC_ReadString:            	
					int length;
					length = machine->ReadRegister(5);
					char* buffer;
					ReadString(buffer,length);
					break;
				// Khi nguoi dung goi syscall PrintString.
				case SC_PrintString:            	
					char* buf;
					PrintString(buf);
					break;
		    }
		    incProgCounter();//tang thanh ghi
	}
}
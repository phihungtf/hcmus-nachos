# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as

GCCDIR = ../../../gnu-decstation-ultrix/decstation-ultrix/2.95.3/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc -B../../../gnu-decstation-ultrix/
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt shell matmult sort ReadPrintInt ReadPrintChar ReadPrintString BubbleSort Help ascii

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.c > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

ReadPrintInt.o: ReadPrintInt.c
	$(CC) $(CFLAGS) -c ReadPrintInt.c
ReadPrintInt: ReadPrintInt.o start.o
	$(LD) $(LDFLAGS) start.o ReadPrintInt.o -o ReadPrintInt.coff
	../bin/coff2noff ReadPrintInt.coff ReadPrintInt

ReadPrintChar.o: ReadPrintChar.c
	$(CC) $(CFLAGS) -c ReadPrintChar.c
ReadPrintChar: ReadPrintChar.o start.o
	$(LD) $(LDFLAGS) start.o ReadPrintChar.o -o ReadPrintChar.coff
	../bin/coff2noff ReadPrintChar.coff ReadPrintChar

ReadPrintString.o: ReadPrintString.c
	$(CC) $(CFLAGS) -c ReadPrintString.c
ReadPrintString: ReadPrintString.o start.o
	$(LD) $(LDFLAGS) start.o ReadPrintString.o -o ReadPrintString.coff
	../bin/coff2noff ReadPrintString.coff ReadPrintString

BubbleSort.o: BubbleSort.c
	$(CC) $(CFLAGS) -c BubbleSort.c
BubbleSort: BubbleSort.o start.o
	$(LD) $(LDFLAGS) start.o BubbleSort.o -o BubbleSort.coff
	../bin/coff2noff BubbleSort.coff BubbleSort

Help.o: Help.c
	$(CC) $(CFLAGS) -c Help.c
Help: Help.o start.o
	$(LD) $(LDFLAGS) start.o Help.o -o Help.coff
	../bin/coff2noff Help.coff Help

ascii.o: ascii.c
	$(CC) $(CFLAGS) -c ascii.c
ascii: ascii.o start.o
	$(LD) $(LDFLAGS) start.o ascii.o -o ascii.coff
	../bin/coff2noff ascii.coff ascii

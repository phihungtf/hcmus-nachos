// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#define MAX_FILE 10

#ifdef FILESYS_STUB // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available
class FileSystem
{
private:
	struct FileDescription
	{
		OpenFile *open_file;
		char *name;
	};

public:
	FileDescription *open_f;

	FileSystem()
	{
		open_f = new FileDescription[MAX_FILE];
		for (int i = 0; i < MAX_FILE; i++)
		{
			open_f[i].open_file = NULL;
			open_f[i].name = NULL;
		}
	}
	~FileSystem()
	{
		for (int i = 0; i < MAX_FILE; i++)
		{
			if (open_f[i].open_file != NULL)
			{
				delete open_f[i].open_file;
				open_f[i].open_file = NULL;
			}
			if (open_f[i].name != NULL)
			{
				delete open_f[i].name;
				open_f[i].name = NULL;
			}
		}
		delete[] open_f;
	}
	bool Create(char *name)
	{
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1)
			return FALSE;
		Close(fileDescriptor);
		return TRUE;
	}

	OpenFile *Open(char *name)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
			return NULL;
		return new OpenFile(fileDescriptor);
	}

	int openFile(char *fileName)
	{
		int freeSlot = -1, fileDescriptor;
		for (int i = 2; i < MAX_FILE; i++)
		{
			if (open_f[i].open_file == NULL)
			{
				open_f[i].name = new char[strlen(fileName) + 1];
				for (int j = 0; j < strlen(fileName); j++)
				{
					open_f[i].name[j] = fileName[j];
				}
				open_f[i].name[strlen(fileName)] = '\0';
				freeSlot = i;
				break;
			}
		}
		if (freeSlot == -1)
			return -1;										// if file table is full, return -1
		fileDescriptor = OpenForReadWrite(fileName, FALSE); // create sector
		if (fileDescriptor == -1)
			return -1; // can not create file, return -1
		open_f[freeSlot].open_file = new OpenFile(fileDescriptor);
		return freeSlot;
	}

	int Close(int index)
	{
		if (index < 2 || index >= MAX_FILE)
			return -1;
		if (open_f[index].open_file)
		{
			delete open_f[index].open_file;
			open_f[index].open_file = NULL;
			delete open_f[index].name;
			open_f[index].name = NULL;
			return 0;
		}
		return -1;
	}

	int Read(char *buffer, int numBytes, int index)
	{
		if (index >= MAX_FILE)
			return -1;
		if (open_f[index].open_file == NULL)
			return -1;
		int res = open_f[index].open_file->Read(buffer, numBytes);
		// if cannot read enough bytes return -2
		if (res != numBytes)
			return -2;
		return res;
	}

	int Write(char *buffer, int numBytes, int index)
	{
		if (index >= MAX_FILE)
			return -1;
		if (open_f[index].open_file == NULL)
			return -1;
		return open_f[index].open_file->Write(buffer, numBytes);
	}
};

#else // FILESYS
class FileSystem
{
public:
	FileSystem(bool format); // Initialize the file system.
							 // Must be called *after* "synchDisk"
							 // has been initialized.
							 // If "format", there is nothing on
							 // the disk, so initialize the directory
							 // and the bitmap of free blocks.

	bool Create(char *name, int initialSize);
	// Create a file (UNIX creat)

	OpenFile *Open(char *name); // Open a file (UNIX open)

	bool Remove(char *name); // Delete a file (UNIX unlink)

	void List(); // List all the files in the file system

	void Print(); // List all the files and their contents

private:
	OpenFile *freeMapFile;	 // Bit map of free disk blocks,
							 // represented as a file
	OpenFile *directoryFile; // "Root" directory -- list of
							 // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H

#include "ptable.h"
#include "system.h"

PTable::PTable(int size)
{
	int i = 0;
	psize = size;
	bm = new BitMap(size);
	bmsem = new Semaphore("BMsem",1);
		
	for(i = 0 ; i < MAXPROCESS ; ++i)
		pcb[i] = NULL;
	bm->Mark(0);
}

PTable::~PTable()
{
	int i=0;
	if(bm!=NULL)
		delete bm;
	if(bmsem!=NULL)
		delete bmsem;
	for(i=0; i<MAXPROCESS; i++)
		if(pcb[i]!=NULL)
			delete pcb[i];
}

//--------------------------------------------------------------------

int PTable::ExecUpdate(char* filename)
{
	bmsem->P();			//chi nap 1 tien trinh vao mot thoi diem

//Kiem tra file co ton tai tren may khong
	OpenFile *executable = fileSystem->Open(filename);
	if (executable == NULL) 
	{
		printf("\nPTABLE: Unable to open file %s\n", filename);
		bmsem->V();
		return -1;
  }
	delete executable;			// close file

//Kiem tra chuong trinh duoc goi co la chinh no khong
	if(!strcmp(filename,currentThread->getName()))
	{
		printf("\nLoi: khong duoc phep goi exce chinh no !!!\n");
		bmsem->V();
		return -1;
	}
////////////////////////////////////////////////////////////

//Kiem tra con slot trong khong
	int ID= GetFreeSlot();
	if(ID==-1)
	{
		printf("\nLoi: Da vuot qua 10 tien trinh !!!\n");
		bmsem->V();
		return -1;
	}
////////////////////////////////////////////////////////////
	pcb[ID]= new PCB(ID);
	bm->Mark(ID);
	int pID= pcb[ID]->Exec(filename,ID);

	bmsem->V();
	return pID;
}

int PTable::ExitUpdate(int ec)
{
//Kiem tra pID co ton tai khong
	int pID= currentThread->processID;
	if(!IsExist(pID))
	{
		printf("\nLoi: Tien trinh khong ton tai !!!\n");
		return -1;
	}
//////////////////////////////////////////////////////////////

//Neu la main process thi Halt()
	if(pID==0)
	{
		interrupt->Halt();
		return 0;
	}
/////////////////////////////////////////////////////////////

	pcb[pID]->SetExitCode(ec);
	
	if(pcb[pID]->JoinStatus != -1)
	{
		pcb[pID]->JoinRelease();
		pcb[pID]->ExitWait();
		Remove(pID);	
	}
	else
		Remove(pID);
	return ec;
}

int PTable::JoinUpdate(int pID)
{
	
	if(pID <= 0 || pID > 9)
	{
		printf("\nLoi: Khong ton tai process: id = %d\n",pID);
		return -1;
	}

	if (pcb[pID] == NULL)
	{
		printf("Loi: Khong ton tai process id nay!");
		return -1;
	}

//kiem tra tien trinh dang chay co la cha cua tien trinh can join hay khong
	if(currentThread->processID != pcb[pID]->parentID)
	{
		printf("\nLoi: Ko duoc phep join vao tien trinh khong phai cha cua no !!!\n");
		return -1;
	}
/////////////////////////////////////////////////////////////////////////////////////////////
	

	pcb[pID]->JoinWait(); 	//doi den khi tien trinh con ket thuc

	int ec = pcb[pID]->GetExitCode();

	if(ec != 0)
	{
		printf("\nProcess exit with exitcode EC = %d ",ec);
		return -1;
	}

	pcb[pID]->ExitRelease();	//cho phep tien trinh con ket thuc
	
	return 0;
}

void PTable::Remove(int pID)
{
	if(pID<0 || pID>9)
		return;
	if(bm->Test(pID))
	{
		bm->Clear(pID);
		delete pcb[pID];
	}
}

//----------------------------------------------------------------------------------------------
int PTable::GetFreeSlot()
{
	return bm->Find();
}

bool PTable::IsExist(int pID)
{
	if(pID<0 || pID>9)
		return 0;
	return bm->Test(pID);
}

char* PTable::GetName(int pID)
{
	if(pID>=0 && pID<10 && bm->Test(pID))
		return pcb[pID]->GetNameThread();
}

Sem::Sem(char*na, int i) {
	strcpy(this->name, na);
	sem = new Semaphore(this->name, i);
}
Sem::~Sem(){
	if(sem) delete sem;
}
void Sem::wait(){
	sem->P();	// Down(sem)
}
void Sem::signal(){
	sem->V();	// Up(sem)
}
char* Sem::GetName(){
	return this->name;
}

// Constructor
STable::STable()
{	
	this->bm = new BitMap(MAX_SEMAPHORE);
	
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		this->semTab[i] = NULL;
	}
}

// Destructor
STable::~STable()
{
	if(this->bm)
	{
		delete this->bm;
		this->bm = NULL;
	}
	for(int i=0; i < MAX_SEMAPHORE; i++)
	{
		if(this->semTab[i])
		{
			delete this->semTab[i];
			this->semTab[i] = NULL;
		}
	}
	
}

int STable::Create(char *name, int init)
{

	// Check da ton tai semaphore nay chua?
	for(int i=0; i<MAX_SEMAPHORE; i++)
	{
		if(bm->Test(i))
		{
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				return -1;
			}
		}
		
	}
	// Tim slot tren bang semTab trong
	int id = this->FindFreeSlot();
	
	// Neu k tim thay thi tra ve -1
	if(id < 0)
	{
		return -1;
	}

	// Neu tim thay slot trong thi nap Semaphore vao semTab[id]
	this->semTab[id] = new Sem(name, init);
	return 0;
}

int STable::Wait(char *name)
{
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		// Kiem tra o thu i da duoc nap semaphore chua
		if(bm->Test(i))
		{
			// Neu co thi tien hanh so sanh name voi name cua semaphore trong semTab
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Neu ton tai thi cho semaphore down(); 
				semTab[i]->wait();
				return 0;
			}
		}
	}
	printf("Khong ton tai semaphore");
	return -1;
}

int STable::Signal(char *name)
{
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		// Kiem tra o thu i da duoc nap semaphore chua
		if(bm->Test(i))
		{
			// Neu co thi tien hanh so sanh name voi name cua semaphore trong semTab
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Neu ton tai thi cho semaphore up(); 
				semTab[i]->signal();
				return 0;
			}
		}
	}
	printf("Khong ton tai semaphore");
	return -1;
}

int STable::FindFreeSlot()
{
	return this->bm->Find();
}

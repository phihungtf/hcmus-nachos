#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"
#include "synch.h"
#include "bitmap.h"

#define MAX_SEMAPHORE 10
#define MAXPROCESS 10

class PTable
{
private:
	BitMap		*bm;
	PCB		*pcb[MAXPROCESS];
	int		psize;
	Semaphore	*bmsem;		//Dung de ngan chan truong hop nap 2 tien trinh cung luc

public:
	PTable(int size);		//Khoi tao size doi tuong pcb de luu size process. Gan gia tri ban dau la null. Nho khoi tao *bm va *bmsem de su dung
	~PTable();			//Huy doi tuong da tao
	int ExecUpdate(char* filename);		//return PID
	int ExitUpdate(int ec);
	int JoinUpdate(int pID);
	int GetFreeSlot();		//Tim slot trong de luu thong tin cho tien trinh moi
	bool IsExist(int pID); 	//Kiem tra co ton tai process ID nay khong
	void Remove(int pID); 	//Xoa mot processID ra khoi mang quan ly no, khi ma tien trinh nay da ket thuc
	char* GetName(int pID);	//Lay ten cua tien trinh co processID la pID
};



// Lop Sem dung de quan ly semaphore.
class Sem {
private:
	char name[50];		// Ten cua semaphore
	Semaphore* sem;		// Tao semaphore de quan ly
public:
	// Khoi tao doi tuong Sem. Gan gia tri ban dau la null
	// Nho khoi tao sem su dung
	Sem(char* na, int i);
	~Sem();
	void wait();
	void signal();
	char* GetName();
};

class STable{
private:
	BitMap* bm;	// quản lý slot trống
	Sem* semTab[MAX_SEMAPHORE];	// quản lý tối đa 10 đối tượng Sem
public:
	//khởi tạo size đối tượng Sem để quản lý 10 Semaphore. Gán giá trị ban đầu là null
	// nhớ khởi tạo bm để sử dụng
	STable();		

	~STable();	// hủy các đối tượng đã tạo
	// Kiểm tra Semaphore “name” chưa tồn tại thì tạo Semaphore mới. Ngược lại, báo lỗi.
	int Create(char *name, int init);

	// Nếu tồn tại Semaphore “name” thì gọi this->P()để thực thi. Ngược lại, báo lỗi.
	int Wait(char *name);

	// Nếu tồn tại Semaphore “name” thì gọi this->V()để thực thi. Ngược lại, báo lỗi.
	int Signal(char *name);
	
	// Tìm slot trống.
	int FindFreeSlot();

	
};
#endif

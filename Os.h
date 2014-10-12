#ifndef  _OS_CLASS_H
#define  _OS_CLASS_H
#include "Assembler.h"
#include "VirtualMachine.h"
#include "PCB.h"
#include <queue>
#include <list>
#include <vector>
#define  VM_RET_STATUS     ((vMach.SR & 0x000000e0) >> 5)
#define  IO_REGISTER       ((vMach.SR & 0x00000300) >> 8)
#define  READ              6
#define  WRITE             7
#define  TIME_SLICE        0  
#define  OVERFLOW          ((vMach.SR & 0x00000010) >> 4) == 1
#define  PAGE_FAULT        ((vMach.SR & 0x400) >> 10) == 1
using namespace std;

class os {
public:

  os();
  os(Algorithm);
  bool compileSrcFiles();
  bool loadObjFiles();
  void loadNextProcess();
  void saveProcesState();
  void runScheduler();
  void queryWaitQueue();
  void IO_trap();
  void SYS_trap();
  void outputStatistics();
  void pushIntoReadyQ(PCB*);
  void loadPage(int, int,int);
  void scheduleJobs(int);
  void freeMemory();
  int searchfreeFrames(PCB*);
  int repAlgorithm(PCB*);
  void writePage(int, int, int);
  void stack_frameRequest(int);
  PCB* findProcess(int);
  void readyQpushFront( PCB* );
  void loadStacktoMem();  

  string objFile;
  ifstream input;
  list<PCB*> jobs;
  list<PCB*> tempJobList;
  list<int> freeFrames;
  queue<PCB*> readyQ, waitQ;
  PCB* running;
  VirtualMachine vMach;
  Assembler Asm;
  int idleTime, contextSWcount;
  int faultAddress, faultPage;
  vector<string> objFiles;
  Algorithm replacementAlg;
};
#endif



#ifndef _VIRTUAL_MACHINE_H
#define _VIRTUAL_MACHINE_H
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "PCB.h"
#include "PageTable.h"
#define MEM_SIZE       256
#define REG_FILE_SIZE  4
#define LESS           ((SR & 8) >> 3 )
#define EQUAL          ((SR & 4) >> 2 )
#define GREATER        ((SR & 2) >> 1 )
#define CARRY          (SR & 1)
#define MSB            ((R[rd] & 0x00008000) >> 15 )
#define LSB            (R[rd] & 1)
#define RS_MSB         ((R[rs] & 0x00008000) >> 15 )
//#define CONST_MSB      ((constant & 0x8000) >> 15 )
#define CONST_MSB      ((constant & 0x80) >> 7 )
#define CLR_CARRY      SR &= 0xfffffffe 
#define SET_CARRY      SR |= 1
#define SET_OV         SR |= 16
#define CLR_OV         SR &= 0xffffffef
#define SIGN_EXT       R[rd] |= 0xffff0000 //sign extend negative numbers
#define ZERO_EXT       R[rd] &= 0x0000ffff 
#define CARRY_BIT      ((R[rd] & 0x10000) >> 16 )
#define PG_FAULT_BIT   ((SR & 0x400) >> 10 )
using namespace std;

struct format1 {
  unsigned UNUSED:6;
  unsigned RS:2;
  unsigned I:1;
  unsigned RD:2;
  unsigned OP:5;
};

struct format2 {
  unsigned ADDR:8;
  unsigned I:1;
  unsigned RD:2;
  unsigned OP:5;
};

struct format3 {
  int CONST:8;
  unsigned I:1;
  unsigned RD:2;
  unsigned OP:5;
};

union instruction {
  int machCode;
  format1 f1;
  format2 f2;
  format3 f3;
};  

class VirtualMachine{
  
  typedef void (VirtualMachine::*FP)();
  friend class os;

 public:
  VirtualMachine();
  void runProcess(PCB*);
  void coreDump();
  vector<FP> InstSet;
  vector<int> mem;
  vector<int> R;
  instruction instr;
  int IR, PC, SR, SP;
  int BASE, LIMIT, constant;
  int clock, quantum;
  int stack_top;
  unsigned op, rd, imode, rs, addr, phy_addr;
  bool STOP, pagedOut;
  PCB* runningProc;
  PageTable TLB;
  bool stackRequest;
  int futureStackFrame;
  Algorithm rep_algorithm;
  vector<int> InvertedTable;
  vector<int> FrameReg;


 private:
  //instruction set
  void load();
  void store();
  void add();
  void addc();
  void sub();
  void subc();
  void And();
  void Xor();
  void Compl() { R[rd] = ~R[rd]; clock++; }
  void shl();
  void shla();
  void shr();
  void shra();
  void compr();
  void getstat() { R[rd] = SR; clock++; }
  void putstat() { SR = R[rd]; clock++; }
  void jump();
  void jumpl(); 
  void jumpe();
  void jumpg();
  void call();
  void Return();
  void read();
  void write();
  void halt();
  void noop()  {}  // end of instruction set

  void checkAddOverflow(int,int);
  void checkSubOverflow(int,int);
  void outOfBounds();
  void decodeInstr();
  void execInstr();
  void pageFault();
  void setNewLimits();
  void checkPageLimits();
  void LRUalgorithm(int);
  void setAlgorithm(Algorithm);
};
#endif

#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#define VALID_RD       (RD >= 0 && RD <= 3)
#define VALID_RS       (RS >= 0 && RS <= 3)
#define VALID_CONST    ( CONST >= -128 && CONST < 128 )
#define VALID_ADDR     ( ADDR >= 0 && ADDR <= 255 )
#define COMMENT        (extraArg[0] == '!')
using namespace std;

class Assembler {
  // define Function Pointer type
  typedef void (Assembler::*FP)(fstream&, istringstream&);
  void Invalid_Arg(string);
  
  void load(fstream&, istringstream& );//each instruction has its own function
  void loadi(fstream&, istringstream& );
  void store(fstream&, istringstream& );
  void add(fstream&, istringstream& );
  void addi(fstream&, istringstream& );
  void addc(fstream&, istringstream& );
  void addci(fstream&, istringstream& );
  void sub(fstream&, istringstream& );
  void subi(fstream&, istringstream& );
  void subc(fstream&, istringstream& );
  void subci(fstream&, istringstream& );
  void And(fstream&, istringstream& );
  void Andi(fstream&, istringstream& );
  void Xor(fstream&, istringstream& );
  void Xori(fstream&, istringstream& );
  void Compl(fstream&, istringstream& );
  void shl(fstream&, istringstream& );
  void shla(fstream&, istringstream& );
  void shr(fstream&, istringstream& );
  void shra(fstream&, istringstream& );
  void compr(fstream&, istringstream& );
  void compri(fstream&, istringstream& );
  void getstat(fstream&, istringstream& );
  void putstat(fstream&, istringstream& );
  void jump(fstream&, istringstream& );
  void jumpl(fstream&, istringstream& );
  void jumpe(fstream&, istringstream& );
  void jumpg(fstream&, istringstream& );
  void call(fstream&, istringstream& );
  void Return(fstream&, istringstream& );
  void read(fstream&, istringstream& );
  void write(fstream&, istringstream& );
  void halt(fstream&, istringstream& );
  void noop(fstream&, istringstream& );

  map<string, FP> instr_set; // the map
  unsigned LineNumber, ProgSize;
  int RD, RS, CONST, ADDR;
  string opcode, extraArg;
  bool otherError;
 public:

  Assembler();
  int assemble(string);
};
#endif

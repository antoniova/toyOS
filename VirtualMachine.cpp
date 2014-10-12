#include "VirtualMachine.h"

VirtualMachine::VirtualMachine()
{
  InstSet.push_back(&VirtualMachine::load);
  InstSet.push_back(&VirtualMachine::store);
  InstSet.push_back(&VirtualMachine::add);
  InstSet.push_back(&VirtualMachine::addc);
  InstSet.push_back(&VirtualMachine::sub);
  InstSet.push_back(&VirtualMachine::subc);
  InstSet.push_back(&VirtualMachine::And);
  InstSet.push_back(&VirtualMachine::Xor);
  InstSet.push_back(&VirtualMachine::Compl);
  InstSet.push_back(&VirtualMachine::shl);
  InstSet.push_back(&VirtualMachine::shla);
  InstSet.push_back(&VirtualMachine::shr);
  InstSet.push_back(&VirtualMachine::shra);
  InstSet.push_back(&VirtualMachine::compr);
  InstSet.push_back(&VirtualMachine::getstat);
  InstSet.push_back(&VirtualMachine::putstat);
  InstSet.push_back(&VirtualMachine::jump);
  InstSet.push_back(&VirtualMachine::jumpl);
  InstSet.push_back(&VirtualMachine::jumpe);
  InstSet.push_back(&VirtualMachine::jumpg);
  InstSet.push_back(&VirtualMachine::call);
  InstSet.push_back(&VirtualMachine::Return);
  InstSet.push_back(&VirtualMachine::read);
  InstSet.push_back(&VirtualMachine::write);
  InstSet.push_back(&VirtualMachine::halt);
  InstSet.push_back(&VirtualMachine::noop);
  STOP = false;
  mem = vector<int>(MEM_SIZE);
  R = vector<int>(REG_FILE_SIZE);
  InvertedTable = vector<int>(32,0);
  FrameReg = vector<int>(32,0);
  BASE = LIMIT = PC = IR = SR = 0;
  op = rd = imode = rs = addr = constant = 0;
  phy_addr = 0;
  SP = stack_top = 256;
  instr.machCode = 0;
  clock = quantum = 0;
  futureStackFrame = 0;
  stackRequest = false;
  pagedOut = false;
}

void VirtualMachine::setAlgorithm(Algorithm alg)
{
  rep_algorithm = alg;
}

void VirtualMachine::coreDump()
{
  for( int i = 0; i < mem.size(); i++ )
    cout << mem[i] << endl;
}

/**********************
This is the main loop of the virtual machine.
**********************/
void VirtualMachine::runProcess(PCB* runningProc)
{
  //  runningProc = p;
  quantum = runningProc->timeSlice + clock;

  // check if process is resuming after a page fault
  if( PG_FAULT_BIT == 1 ){
      SR &= 0xfbff;      //clear page-fault bit
      setNewLimits();    
      if( pagedOut == false )
	  goto repeat_instr;
      pagedOut = false;
  }
  
  while( STOP == false ){
      if( clock >= quantum ){
	  SR = (SR & 0xff1f);//set return-status: time slice done
	  break;
      }
      phy_addr = TLB[PC];
      if( phy_addr == -1 ){  // while the process was away, was its current
	  pageFault();       // page overwritten by some other process?
	  pagedOut = true;   // if it was, then we must load it again.
	  return;            // 'pagedOut' flag determines where execution resumes.
      }
      IR = mem[phy_addr];
      LRUalgorithm(phy_addr);
      PC++;
      if( PC >= LIMIT ){
	  switch( TLB[PC] ){
	     case -1: pageFault();
	       return;
	     case -2: outOfBounds();
	       return;
	     default: setNewLimits();
	  }
      }
repeat_instr:
      decodeInstr();
      execInstr();
  }
}

/****************************
As its name implies, this function extracts
the relevant data from each instruction in memory
*******************************/
void VirtualMachine::decodeInstr()
{
  instr.machCode = IR;
  op = instr.f1.OP;
  rd = instr.f1.RD;
  imode = instr.f1.I;
  rs = instr.f1.RS;
  addr = instr.f2.ADDR;
  constant = instr.f3.CONST;
}

void VirtualMachine::execInstr()
{
  if( op < 0 || op >= InstSet.size() ){//check for valid opcode
      SR = (SR & 0xff1f)|(5 << 5);//set VM return-status
      STOP = true;
  }else
      (this->*InstSet[op])(); 
}

void VirtualMachine::load()
{
  if( imode == 0 ){ // load
      phy_addr = TLB[addr];
      switch( phy_addr ){
         case -1: pageFault();
	   break;
         case -2: outOfBounds();
	   break;
         default: R[rd] = mem[phy_addr];
	          LRUalgorithm(phy_addr);
		  clock += 4;
      }
  }else{  // loadi 
      R[rd] = constant;
      clock++;
  }
}

void VirtualMachine::store()
{
  phy_addr = TLB[addr];
  switch( phy_addr ){
     case -1: pageFault();
       break;
     case -2: outOfBounds();
       break;
     default: mem[phy_addr] = R[rd];
              TLB.table[(addr >> 3)] |= 1; // set modify bit
	      LRUalgorithm(phy_addr);
              clock += 4;
  }
}

void VirtualMachine::add()
{
  int oldMSB = MSB; //R[rd]'s most sig. bit before add
  if( imode == 0 ){
      R[rd] += R[rs];
      checkAddOverflow( oldMSB, RS_MSB );
  }else{
      R[rd] += constant;
      checkAddOverflow( oldMSB, CONST_MSB );
  }
  if( CARRY_BIT == 1 )//look at 17th bit for carry
      SET_CARRY;
  else
      CLR_CARRY;
  clock++;
}

void VirtualMachine::addc()
{
  int oldMSB = MSB; //R[rd]'s most sig. bit before add
  if( imode == 0 ){
      R[rd] = R[rd] + R[rs] + CARRY;
      checkAddOverflow( oldMSB, RS_MSB );
  }else{
      R[rd] = R[rd] + constant + CARRY;
      checkAddOverflow( oldMSB, CONST_MSB );
  }
  if( CARRY_BIT == 1)
      SET_CARRY;
  else
      CLR_CARRY;
  clock++;
}

void VirtualMachine::sub()
{
  int oldMSB = MSB; //R[rd]'s most sig. bit before sub
  if( imode == 0 ){
      R[rd] -= R[rs];
      checkSubOverflow( oldMSB, RS_MSB );
  }else{
      R[rd] -= constant;
      checkSubOverflow( oldMSB, CONST_MSB );
  }
  if( CARRY_BIT == 1 )
      SET_CARRY;
  else
      CLR_CARRY;
  clock++;
}

void VirtualMachine::subc()
{
  int oldMSB = MSB; //R[rd]'s most sig. bit before subc
  if( imode == 0 ){
      R[rd] = R[rd] - R[rs] - CARRY;
      checkSubOverflow( oldMSB, RS_MSB );
  }else{
      R[rd] = R[rd] - constant - CARRY;
      checkSubOverflow( oldMSB, CONST_MSB );
  }
  if( CARRY_BIT == 1 )
      SET_CARRY;
  else
      CLR_CARRY;
  clock++;
}

void VirtualMachine::And()
{
  if( imode == 0 )
      R[rd] &= R[rs];
  else
      R[rd] &= constant;
  clock++;
}

void VirtualMachine::Xor()
{
  if( imode == 0 )
      R[rd] ^= R[rs];
  else
      R[rd] ^= constant;
  clock++;
}

/************************
Since there's no way to catch the out-going bit after 
the shift, we look at the LSB, or MSB, before shifting.
The same applies to all shift instructions.
************************/
void VirtualMachine::shr()
{
  if( LSB == 1 )
      SET_CARRY; //set carry bit
  else
      CLR_CARRY;
  R[rd] &= 0x0000ffff; // clear upper 16 bits
  R[rd] >>= 1;
  clock++;
}

void VirtualMachine::shl()
{
  if( MSB == 1 )
      SET_CARRY;  // set carry bit
  else
      CLR_CARRY;
  R[rd] <<= 1;
  clock++;
}

void VirtualMachine::shra()
{
  if( LSB == 1 )
      SET_CARRY;
  else
      CLR_CARRY;
  if( MSB == 1 ){
      R[rd] >>= 1;
      R[rd] |= 0x8000; //set MSB to 1
  }else{
      ZERO_EXT;
      R[rd] >>= 1;
  }
  clock++;
}

void VirtualMachine::shla()
{
  if( MSB == 1 ){
      SET_CARRY;
      R[rd] <<=1 ;
      R[rd] |= 0x8000; // set MSB to 1
  }else{
      CLR_CARRY;
      R[rd] <<= 1;
      R[rd] &= 0xffff7fff; //clear MSB (set it to 0)
  }
}

void VirtualMachine::compr()
{
  int mask = 17; //bit pattern: 10001
  SR &= mask; //clear L, E, and G bits. Keep V and C bits untouched.
  if( imode == 0 ){
      if( R[rd] < R[rs] )
	  mask = 8;
      if( R[rd] == R[rs] )
	  mask = 4;
      if( R[rd] > R[rs] )
	  mask = 2;
  }else{
      if( R[rd] < constant )
          mask = 8;
      if( R[rd] == constant )
          mask = 4;
      if( R[rd] > constant )
          mask = 2;
  }
  SR |= mask; //set the right bits
  clock++;
}

void VirtualMachine::jump()
{
  PC = addr;
  checkPageLimits();
  clock++;
}

void VirtualMachine::jumpl()
{
  if( LESS == 1 ){
      PC = addr;
      checkPageLimits();
      clock++;
  }
}

void VirtualMachine::jumpe()
{
  if( EQUAL == 1){
      PC = addr;
      checkPageLimits();
      clock++;
  }
}

void VirtualMachine::jumpg()
{
  if( GREATER == 1){
      PC = addr;
      checkPageLimits();
      clock++;
  }
}

void VirtualMachine::call()
{
  //check for stack overflow
  if( ((SP - TLB.progSize)/8) <= 2 ){
    SR = (SR & 0xff1f) | (3 << 5);   //VM return status: stack overflow
    STOP = true;
    return;
  }

  if( addr < BASE || addr >= LIMIT ){
      switch( TLB[addr] ){
         case -1: pageFault();
	     return;
         case -2: outOfBounds();
	     return;
         default: setNewLimits();
      }
  }
  futureStackFrame = ( (SP-6) >> 3 );
  // examine inverted table entry. If frame 
  // is unused ( pid == 0 ), let stack grow.
  if( (InvertedTable[futureStackFrame] & 0xff) == 0 ){
      mem[--SP] = PC;            // push PC
      for(int i = 0; i < 4 ; i++)
	  mem[--SP] = R[i];      // push registers
      mem[--SP] = SR;            // push SR
      PC = addr;
      if( SP < stack_top )
	  stack_top = SP;
      clock += 4;
  }else{
      pageFault();
      stackRequest = true;
  }
}

void VirtualMachine::Return()
{
  int temp = mem[SP+5]; 
  // check whether the page we are returning to 
  // hasn't been paged out or not
  phy_addr = TLB[temp];

  switch( phy_addr ){    
    case -1: pageFault();
       return;
    case -2: outOfBounds();
       return;
  //default: setNewLimits();
  }
  
  if( SP < 256){
      SR = mem[SP++];
      for( int i = 3; i >= 0; i-- )
	  R[i] = mem[SP++];
      PC = mem[SP++];
      setNewLimits();
      clock += 4;
  }else{
      SR = (SR & 0xff1f)|(4 << 5);// VM return-status: stack underflow
      STOP = true;
  }
}

void VirtualMachine::read()
{
  SR = (SR & 0xfc1f)|(6 << 5)|(rd << 8);//set VM return-stat & set I/O register
  STOP = true;
  clock++;
}
      
void VirtualMachine::write()
{
  if( MSB == 1 )
      SIGN_EXT; // if negative, sign extend.
  else
      ZERO_EXT;
  SR = (SR & 0xfc1f)|(7 << 5)|(rd << 8);//set VM return-status & I/O register
  STOP = true;
  clock++;
}

void VirtualMachine::halt()
{ 
  STOP = true; 
  SR = (SR & 0xff1f)|(1 << 5);//set VM return-status
  clock++;
}

void VirtualMachine::checkAddOverflow(int oldMSB, int otherMSB )
{
  if( oldMSB == otherMSB )
      if( oldMSB != MSB )
	  SET_OV;
      else
	  CLR_OV;
}

void VirtualMachine::checkSubOverflow(int oldMSB, int otherMSB )
{
  if( oldMSB != otherMSB )
      if( oldMSB != MSB )
	  SET_OV;
      else
	  CLR_OV;
}

void VirtualMachine::outOfBounds()
{
  SR = (SR & 0xff1f)|(2 << 5);//set VM return-status
  STOP = true;
}

void VirtualMachine::pageFault()
{
  SR = (SR & 0xff1f);// set return-status = 0
  SR |= 0x400;       // set page fault bit
  STOP = true;  
}

void VirtualMachine::setNewLimits()
{
  LIMIT = ((PC & 0xf8) | 7) + 1;
  BASE = LIMIT - 8;
}

void VirtualMachine::checkPageLimits()
{
  if( PC < BASE || PC >= LIMIT ){
      switch( TLB[PC] ){
         case -1: pageFault();
	   break;
         case -2: outOfBounds();
	   break;
         default: setNewLimits();
      }
  }
}

void VirtualMachine::LRUalgorithm(int addr)
{
  if( rep_algorithm == LRU )
      if( (addr >> 3) >= 0 && (addr >> 3))
	  FrameReg[(addr >> 3)] = clock;
}
  

#include "PCB.h"

PCB::PCB( string name,int id )
{
  PID = id;
  p_name = name.substr( 0, name.length() - 2 );//remove ".o" from name
  PC = SR = 0;
  SP = 256;
  IR = 0; // added temporarily
  BASE = 0;
  LIMIT = 8;
  timeSlice = 25;
  CPUtime = waitTime = 0;
  turnAroundTime = IOtime = 0;
  interrptTime = 0;
  stackSize = 0;
  oldTime = 0;
  topOfStack = 256;
  R = vector<int>(REG_FILE_SIZE);
  pagedOut = false;
}

bool PCB::initIO()
{
  string temp = p_name + ".in";
  diskIn.open( temp.c_str() );
  temp = p_name + ".out";
  diskOut.open( temp.c_str() );
  temp = p_name + ".st";
  stackFile.open( temp.c_str(), fstream::in | fstream::out | fstream::trunc );
  temp = p_name + ".o";
  objFile.open( temp.c_str(), fstream::in | fstream::out );
  if( !diskIn.good() || !diskOut.good() || !stackFile.good() || !objFile.good() ){
      cout << "Encountered error while initializing IO.\n" 
	   << "Make sure all prerequisite files are present.\n";
      return false;
  }
  return true;
}

void PCB::freeResources()
{
  diskIn.close();
  diskOut.close();
  stackFile.close();
  objFile.close();
}

void PCB::setupPgTable()
{
  int temp;
  int size = 0;
  objFile.seekg(0);
  objFile >> temp;
  while( !objFile.eof() ){
      size++;
      objFile >> temp;
  }
  objFile.clear(); //clear error flags
  pgTable.progSize = size;
  pgTable.table = vector<int>( size/8 + 1);
}

/*************************************
Name:        Antonio Gonzalez
Course:      CSE 460: Operating Sytems
Lab :        Lab #3 : Phase 3
Assignment:  This lab consists of the third phase in a series of labs
             that will ultimately result in the design and implementation
             of a simplified operating system. In this phase, in addition
       	     to the features already added in previous labs, memory management 
             will be added to the operating system. Our memory management 
             scheme will use demand paging and, thus, a page replacement
             algorithm will be required.
**************************************/
#include "Os.h"
using namespace std;

int main(int argc, char* argv[])
{
  Algorithm rep_algorithm;
  string arg;
  if( argv[1] != NULL )
      arg = argv[1];
  else
      arg = "error";

  if( arg == "-fifo" )
      rep_algorithm = FIFO;
  else if( arg == "-lru" )
      rep_algorithm = LRU;
  else{
      cout << "Invalid option." << endl 
	   << "Options: -fifo   to run using the FIFO replacement algorithm\n"
	   << "         -lru    to run using the LRU replacement algorithm"
	   << endl;
      return -1;
  }
  os myOS(rep_algorithm);

  if( !myOS.compileSrcFiles() )
      return 1;
  if( !myOS.loadObjFiles() )
      return 1;
  myOS.runScheduler();

  return 0;
}

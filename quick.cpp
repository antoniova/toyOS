#include <iostream>
#include <queue>
#include "PCB.h"
using namespace std;

int main()
{
  queue<PCB *> q;
  PCB * x;
  x = q.front();

  if( x == NULL)
    cout << "null pointer" << endl;

}

#ifndef _PAGE_TABLE_H_
#define _PAGE_TABLE_H_
#include <vector>
#define  VALID_BIT      ((table[pageNbr] & 2) >> 1)
#define  MODIFY_BIT     (table[pageNbr] & 0x1)
using namespace std;

class PageTable{
  friend class PCB;
  friend class Os;
 public:
  PageTable();
  PageTable& operator=(PageTable&);
  int operator[](int);
  int getFaultAddress() { return faultAddr; }
  double getHitRatio();
  
  vector<int> table;
  int pageNbr;
  int progSize;
  int faultAddr;
  int references;
  int pagefaults, hits;
};

#endif

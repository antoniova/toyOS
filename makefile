
os: main.o Os.o Assembler.o VirtualMachine.o PCB.o PageTable.o
	g++ -g -o os main.cpp Assembler.o VirtualMachine.o PCB.o Os.o PageTable.o
Assembler.o: Assembler.cpp Assembler.h
	g++ -c -g Assembler.cpp
VirtualMachine.o: VirtualMachine.cpp VirtualMachine.h
	g++ -c -g VirtualMachine.cpp
PCB.o: PCB.cpp PCB.h
	g++ -c -g PCB.cpp
PageTable.o: PageTable.cpp PageTable.h
	g++ -c -g PageTable.cpp
Os.o: Os.cpp Os.h
	g++ -c -g Os.cpp
main.o: main.cpp
	g++ -c -g main.cpp

clean: 
	rm Assembler.o VirtualMachine.o PCB.o os

!Antonio Gonzalez
!CSE460 : Operating Systems
!This is the first, real, working program
!for my assembler. It doesn't really do
!much but hey, it works!!!


!main routine
	loadi 0 9	!line 0
	loadi 1 10
	loadi 2 11
	loadi 3 12	!line 3
	call 12		!call subroutine
	write 0		!line 5
	write 1
	write 2
	write 3
	load 3 32
	write 3
	halt		!line 11
!end of main routine
	

!beginning of subroutine
	loadi 0 5 	!line 12
	loadi 1 6
	loadi 2 7
	loadi 3 8	!line 15
	call 22	
	write 0
	write 1
	write 2		!line 19
	write 3
	return		!line 21
!end of subroutine
	
!another subroutine
	loadi 0 1	!line 22
	loadi 1 2
	loadi 2 3
	loadi 3 4	!line 25
	write 0
	write 1
	write 2
	write 3
	store 3 32
	return		!line 31
	noop		!line 32: store value of R3
!end of subroutine
	
!end of program
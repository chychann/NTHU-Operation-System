#include "syscall.h"

int
main()
{
	int n;
	for (n=15;n<5000;n++) {
		PrintInt(n);
	}
	
	return 0;
        //Halt();
}
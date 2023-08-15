// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
void
ExceptionHandler(ExceptionType which)
{
    char ch;
    int val;
    int type = kernel->machine->ReadRegister(2);
    
    int status, exit, threadID, programID, fileID, numChar;
    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
    DEBUG(dbgTraCode, "In ExceptionHandler(), Received Exception " << which << " type: " << type << ", " << kernel->stats->totalTicks);
    switch (which) {
    case SyscallException:
	switch(type) {
 
	    case SC_Halt:
		DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
		SysHalt();
		cout<<"in exception\n";
		ASSERTNOTREACHED();
	    break;
         
	    case SC_PrintInt:
		DEBUG(dbgSys, "Print Int\n");
		val=kernel->machine->ReadRegister(4);
		DEBUG(dbgTraCode, "In ExceptionHandler(), into SysPrintInt, " << kernel->stats->totalTicks);    
		SysPrintInt(val); 	
		DEBUG(dbgTraCode, "In ExceptionHandler(), return from SysPrintInt, " << kernel->stats->totalTicks);
		// Set Program Counter
		kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
		kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		return;
		ASSERTNOTREACHED();
	    break;
         
	    case SC_MSG:
		DEBUG(dbgSys, "Message received.\n");
		val = kernel->machine->ReadRegister(4);
		{
		char *msg = &(kernel->machine->mainMemory[val]);
		cout << msg << endl;
		}
		SysHalt();
		ASSERTNOTREACHED();
	    break;
         
	    case SC_Create:
		val = kernel->machine->ReadRegister(4);
		{
		char *filename = &(kernel->machine->mainMemory[val]);
		//cout << filename << endl;
		status = SysCreate(filename);
		kernel->machine->WriteRegister(2, (int) status);
		}
		kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
		kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		return;
    ASSERTNOTREACHED();
	    break;
//----------------------------  
//以下是自己寫的
      case SC_Open://must maintain OpenFileTable and deal with invalid open request!
		  DEBUG(dbgSys, "Open\n");
		  val = kernel->machine->ReadRegister(4);
      {
		  char* filename = &(kernel->machine->mainMemory[val]);
		  DEBUG(dbgSys, "File Name:"<<filename<<"\n");
		  status = SysOpen(filename);
		  kernel->machine->WriteRegister(2, (int)status); 
      }
		  //set PC 
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  return;
		  ASSERTNOTREACHED();
		  break;
      
      case SC_Close:
      DEBUG(dbgSys,"Close\n");
		  fileID = kernel->machine->ReadRegister(4);
      {
		  DEBUG(dbgSys, "File ID:"<<fileID<<"\n");
      status=SysClose(fileID);
      kernel->machine->WriteRegister(2, (int)status);
      }
      
		  //set PC 
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  return;
		  ASSERTNOTREACHED();
      break;
  //----------------------------HW2
     /* case MemoryLimitException:
      DEBUG(dbgSys, "Out of Mememory!.\n");
      
      //addrspace->~AddrSpace();      
		  
		  cout<<"in exception\n";
      SysHalt();
		  ASSERTNOTREACHED();
	    break;
      
      */
  //-----------------
      case SC_Read:
      DEBUG(dbgSys, "Enter Read.\n");
      val=kernel->machine->ReadRegister(4);//去reg4取檔名，依照trace SC_Creat可知檔案名用指標存在reg $4
      {
      char *buffer = &(kernel->machine->mainMemory[val]); //取檔名並存起來
      DEBUG(dbgSys, "Read's buffer"<<buffer<<".\n");
      numChar=kernel->machine->ReadRegister(5); //reg $5存需write的size
      fileID=kernel->machine->ReadRegister(6);//reg $6存需write的fileID
      status=SysRead(buffer,numChar,fileID);//呼叫寫在ksyscall.h的SysRead
      DEBUG(dbgSys, "file ID"<<fileID<<".\n");
      kernel->machine->WriteRegister(2, (int) status);//把結果存入reg $2
      }
      
      // Set Program Counter
      kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
      return;
      ASSERTNOTREACHED();
      break;
  //-----------------
      case SC_Write:
      DEBUG(dbgSys, "Enter write.\n");
      val=kernel->machine->ReadRegister(4);//去reg4取檔名，依照trace SC_Creat可知檔案名用指標存在reg $4
      {
      char *buffer = &(kernel->machine->mainMemory[val]); //取檔名並存起來
      DEBUG(dbgSys, "Write's buffer"<<buffer<<".\n");
      numChar=kernel->machine->ReadRegister(5); //reg $5存需write的size
      fileID=kernel->machine->ReadRegister(6);//reg $6存需write的fileID
      status=SysWrite(buffer,numChar,fileID);//呼叫寫在ksyscall.h的SysWrite
      kernel->machine->WriteRegister(2, (int) status);//把結果存入reg $2
      }
      
      // Set Program Counter
      kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
      return;
      ASSERTNOTREACHED();
      break;
//----------------------------

    case SC_Add:
		DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
		/* Process SysAdd Systemcall*/
		int result;
		result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
		/* int op2 */(int)kernel->machine->ReadRegister(5));
		DEBUG(dbgSys, "Add returning with " << result << "\n");
		/* Prepare Result */
		kernel->machine->WriteRegister(2, (int)result);	
		/* Modify return point */
		{
		/* set previous programm counter (debugging only)*/
		kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			
		/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
		/* set next programm counter for brach execution */
		kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		cout << "result is " << result << "\n";	
		return;	
		ASSERTNOTREACHED();
	    break;
         
	    case SC_Exit:
			DEBUG(dbgAddr, "Program exit\n");
      val=kernel->machine->ReadRegister(4);
      cout << "return value:" << val << endl;
			kernel->currentThread->Finish();
            break;
   
      
      
            
            
            default:
		cerr << "Unexpected system call " << type << "\n";
	    break;
         
         
  
  
	}
	break;
	
  default:
		cerr << "Unexpected user mode exception " << (int)which << "\n";
		break;
    }
    
    ASSERTNOTREACHED();
}


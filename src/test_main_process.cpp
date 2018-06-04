#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include "Ipcctl.h"
#include "CMsgProcess.h"

#define bTrue  1

#define bFalse  0
#define   bufSize  (15)
char  stdInCtrl[bufSize];
bool testRun=true;
const char *stopTest="quitetest";

int main(int argc, char **argv)
{
	   int Ret;
	    CMsgProcess  mProc;
	    fprintf(stdout, "start!!!!\r\n");
	    int shm_perm[IPC_MAX];
	    shm_perm[IPC_SHA] = shm_rdonly;
	    shm_perm[IPC_OSD_SHA] = shm_rdwr;
	    shm_perm[IPC_UTCTRK_SHA] = shm_rdwr;
        Ipc_init();
        Ipc_create(shm_perm);
        mProc.Create();
        fprintf(stdout, "Create sucess!\r\n");
        mProc.Init();
        mProc.Run();

        while(testRun){
        		char *pstr;
        		pstr=fgets(stdInCtrl,10,stdin);
        	    if(!strncmp(stdInCtrl,stopTest,9))
        	    	testRun=false;
        }
        mProc.Destroy();

    return 0;
}


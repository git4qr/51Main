#include "MessageQueueWrapper.h"
#include "SharedMemoryWrapper.h"
#include "SemaphoreWrapper.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include"Ipcctl.h"
IPC_Handl Ipc_Handl[IPC_MAX];

IMGSTATUS * IMGstatus=NULL;
unsigned char *IMGosd=NULL;
void Ipc_init()
{
	Ipc_Handl[0].name=_PATH0_;
	Ipc_Handl[0].Identify=IPC_TOIMG_MSG;
	Ipc_Handl[0].Class=IPC_Class_MSG;
	
	Ipc_Handl[1].name=_PATH1_;
	Ipc_Handl[1].Identify=IPC_FRIMG_MSG;
	Ipc_Handl[1].Class=IPC_Class_MSG;
	
	Ipc_Handl[2].name=_PATH2_;
	Ipc_Handl[2].Identify=IPC_SHA;
	Ipc_Handl[2].Class=IPC_Class_SHA;
	

	Ipc_Handl[3].name=_PATH3_;
	Ipc_Handl[3].Identify=IPC_SEM;
	Ipc_Handl[3].Class=IPC_Class_SEM;

}

int ipc_sendmsg(SENDST* Param,int Mesgthe )
{
	int ret;
	Message msg;
	char buffer[MESSAGE_SIZE];
	memset(buffer,0,sizeof(buffer));
	int mtype=1;
	//buffer[0]=Param->param1;
	//buffer[1]=Param->param2;
       memcpy(buffer,Param,sizeof(SENDST));
	setMessage(&msg, buffer, MESSAGE_SIZE, mtype);
	/* Send message: */
	ret = messageQueueSend(Ipc_Handl[Mesgthe].IPCID, &msg);
	return ret;


}

int ipc_recvmsg(SENDST* Param,int Mesgthe )
{
	int ret;
	Message msg;
	int mtype=1;
	clearMessage(&msg);
	ret = messageQueueReceive(Ipc_Handl[Mesgthe].IPCID, &msg, mtype);
    	memcpy(Param,msg.buffer,sizeof(SENDST));

	return ret;



}
void  ipc_status_P()
{
	semWait(Ipc_Handl[IPC_SEM].IPCID,0);
}
void  ipc_status_V()
{
	semSignal(Ipc_Handl[IPC_SEM].IPCID,0);
}

int ipc_settrack(unsigned int trackstatus, int trackposx, int trackposy)
{
	int ret=0;
	if(IMGstatus==NULL)
		return -1;
//	ret = sharedMemoryLock(Ipc_Handl[IPC_SHA].IPCID);
	ipc_status_P();
	IMGstatus->m_trackstatus=trackstatus;
	IMGstatus->m_trackpos_x=trackposx;
	IMGstatus->m_trackpos_y=trackposy;
	ipc_status_V();
//	ret = sharedMemoryUnlock(Ipc_Handl[IPC_SHA].IPCID);

	return ret;

}

int ipc_setSensorstat(unsigned int unitFaultStat)
{
	int ret=0;
	if(IMGstatus==NULL)
		return -1;
	ipc_status_P();
	IMGstatus->unitFaultStat=unitFaultStat;

	ipc_status_V();

	return ret;

}


int ipc_gettrack(unsigned int* trackstatus, int* trackposx, int* trackposy)
{
	int ret=0;
	if(IMGstatus==NULL)
		return -1;
	if(trackstatus==NULL||trackposx==NULL||trackposy==NULL)
		return -1;
	ipc_status_P();

	//ret = sharedMemoryLock(Ipc_Handl[IPC_SHA].IPCID);
	*trackstatus=IMGstatus->m_trackstatus;
	*trackposx=IMGstatus->m_trackpos_x;
	*trackposy=IMGstatus->m_trackpos_y;
	ipc_status_V();
//	ret = sharedMemoryUnlock(Ipc_Handl[IPC_SHA].IPCID);

	return ret;

}

int ipc_getSensorstat(unsigned int* unitFaultStat)
{
	int ret=0;
	if(IMGstatus==NULL)
		return -1;
	if(unitFaultStat==NULL)
		return -1;
	
	ipc_status_P();

	*unitFaultStat=IMGstatus->unitFaultStat;

	ipc_status_V();

	return ret;
}


void Ipc_create()
{
	for(int i=0;i<IPC_MAX;i++)
		{

			key_t key;
			if(Ipc_Handl[i].Class==IPC_Class_MSG)
					{
						
						key=messageKeyGet(Ipc_Handl[i].name,Ipc_Handl[i].Identify);
						Ipc_Handl[i].IPCID=messageQueueCreate(key);
					}
				else if(Ipc_Handl[i].Class==IPC_Class_SHA)
					{
							if(Ipc_Handl[i].Identify==IPC_SHA)
									{
											key=sharedKeyGet(Ipc_Handl[i].name,Ipc_Handl[i].Identify);
											Ipc_Handl[i].IPCID=sharedMemoryCreateOrGet(key,SHMEMSTATUSSIZE);
											IMGstatus=(IMGSTATUS *)sharedMemoryAttach(Ipc_Handl[i].IPCID);
									}
					}
				else if(Ipc_Handl[i].Class==IPC_Class_SEM)
									{
											if(Ipc_Handl[i].Identify==IPC_SEM)
													{
														key=semKeyGet(Ipc_Handl[i].name,Ipc_Handl[i].Identify);
														Ipc_Handl[i].IPCID=semCreate(key,1);
													}
									}	
		}
}

void Ipc_uninit()
{
	for(int i=0;i<IPC_MAX;i++)
		{
			if(Ipc_Handl[i].Class==IPC_Class_MSG)
				{
					messageQueueDelete(Ipc_Handl[i].IPCID);
				}
			else if(Ipc_Handl[i].Class==IPC_Class_SHA)
				{
					sharedMemoryDelete(Ipc_Handl[i].IPCID);
				}
			else if(Ipc_Handl[i].Class==IPC_Class_SEM)
			{
				semDelete(Ipc_Handl[i].IPCID);
			}
		}
}



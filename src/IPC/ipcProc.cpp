#include "ipcProc.h"
#include "msg_id.h"

extern selectTrack 	m_selectPara;

CIPCProc::CIPCProc()
{
	pthread_mutex_init(&mutex,NULL);
	CMD_AUTOCHECK fr_img_cmd_autocheck = {0,0,0,0};
	CMD_SENSOR fr_img_cmd_sensor={ipc_eSen_TV};
	CMD_PinP fr_img_cmd_pinp = {0,0,0};
	CMD_TRK fr_img_cmd_trk = {0,0};
	CMD_SECTRK fr_img_cmd_sectrk = {0};
	CMD_ENH fr_img_cmd_enh = {0};
	CMD_MTD fr_img_cmd_mtd = {0};
	CMD_MMT fr_img_cmd_mmt = {0};
	CMD_MMTSELECT fr_img_cmd_mmtsel = {0};
	CMD_TRKDOOR fr_img_cmd_trkdoor = {0,1};
	CMD_SYNC422 fr_img_cmd_sync422 = {0};
	CMD_ZOOM fr_img_cmd_zoom = {0};
	SENDST fr_img_test = {0};	trackstatus = 0;
	trackposx=0.f;
	trackposy=0.f;
	exitThreadIPCRcv = false;
	exitGetShamDatainThrd = false;
	pThis = this ;
}
CIPCProc::~CIPCProc()
{
	Destroy();

}

int CIPCProc::Create()
{
	OSA_thrCreate(&thrHandlPCRcv,
			IPC_childdataRcvn,
	                  0,
	                  0,
	                  this);
	 return 0;
}

int CIPCProc::Destroy()
{
	exitThreadIPCRcv=true;
	OSA_thrJoin(&thrHandlPCRcv);
	return 0;
}
void CIPCProc::getIPCMsgProc()
{
	   int getx,gety;
  	   struct timeval tmp;
      	tmp.tv_sec = 0;
      	tmp.tv_usec = 10000;
      	while(!exitThreadIPCRcv){
         ipc_recvmsg(&fr_img_test,IPC_FRIMG_MSG);
        pthread_mutex_lock(&mutex);
        switch(fr_img_test.cmd_ID)
        {
            case trk:
                memcpy(&fr_img_cmd_trk, fr_img_test.param, sizeof(fr_img_cmd_trk));
                break;
            case sectrk:
                memcpy(&fr_img_cmd_sectrk, fr_img_test.param, sizeof(fr_img_cmd_sectrk));
                break;
            case enh:
                memcpy(&fr_img_cmd_enh, fr_img_test.param, sizeof(fr_img_cmd_enh));
                break;
            case mtd:
                memcpy(&fr_img_cmd_mtd, fr_img_test.param, sizeof(fr_img_cmd_mtd));
            case trkdoor:
                memcpy(&fr_img_cmd_trkdoor, fr_img_test.param, sizeof(fr_img_cmd_trkdoor));
                break;
            case mmt:
                memcpy(&fr_img_cmd_mmt, fr_img_test.param, sizeof(fr_img_cmd_mmt));
                break;
            case mmtselect:
                memcpy(&fr_img_cmd_mmtsel, fr_img_test.param, sizeof(fr_img_cmd_mmtsel));
                break;
            case read_shm_trkpos:
        		ipc_gettrack(&trackstatus,&trackposx,&trackposy);//get value from shared_memory
        		Work_quePut(Cmd_IPC_TrkCtrl);
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&mutex);
     }
}

#if 0
void CIPCProc::getShamdataInThrd()
{
	while(!exitGetShamDatainThrd){
		ipc_gettrack(&trackstatus,&trackposx,&trackposy);//get value from shared_memory
	 //printf("get value from shared_memory: trackstatus=%d\t trackposx=%f\t trackposy=%f\n",trackstatus,trackposx,trackposy);
	}
}
#endif

int  CIPCProc::ipcTrackCtrl(volatile unsigned char AvtTrkStat)
{
     memset(test.param,0,PARAMLEN);
	test.cmd_ID = trk;
	//if(AvtTrkStat != fr_img_cmd_trk.AvtTrkStat)
	{
		test.param[0]=AvtTrkStat;
	    	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	}
		return 0;
}

int  CIPCProc::ipcMutilTargetDetecCtrl(volatile unsigned char ImgMmtStat)//1:open 0:close
{
      memset(test.param,0,PARAMLEN);
	test.cmd_ID = mmt;
	if(ImgMmtStat != fr_img_cmd_mmt.ImgMmtStat)
	{
		test.param[0]=ImgMmtStat;
	    	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	}

		return 0;
}

int  CIPCProc::ipcMutilTargetSelectCtrl(volatile unsigned char ImgMmtSelect)
{
      memset(test.param,0,PARAMLEN);
	test.cmd_ID = mmtselect;
	if(ImgMmtSelect != fr_img_cmd_mmtsel.ImgMmtSelect)
	{
		test.param[0]=ImgMmtSelect;
	    	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	}

		return 0;
}

int CIPCProc::ipcImageEnhanceCtrl(volatile unsigned char ImgEnhStat) //1open 0close
{
      memset(test.param,0,PARAMLEN);
	test.cmd_ID = enh;
	if(ImgEnhStat != fr_img_cmd_enh.ImgEnhStat)
	{
		test.param[0]=ImgEnhStat;
	    	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	}
		return 0;
}

int CIPCProc::ipcMoveTatgetDetecCtrl(volatile unsigned char ImgMtdStat)
{
      memset(test.param,0,PARAMLEN);
	test.cmd_ID = mtd;
	if(ImgMtdStat != fr_img_cmd_mtd.ImgMtdStat)
	{
		test.param[0]=ImgMtdStat;
	    	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	}

		return 0;
}

int CIPCProc::ipcSecTrkCtrl(selectTrack *m_selcTrak)
{
	CMD_SECTRK cmd_sectrk;
    memset(test.param,0,PARAMLEN);
	test.cmd_ID = sectrk;
	{
		cmd_sectrk.SecAcqStat = m_selcTrak->SecAcqStat;
	//	cmd_sectrk.ImgPixelX =m_selcTrak->ImgPixelX;
	//	cmd_sectrk.ImgPixelY =m_selcTrak->ImgPixelY;
	//	memcpy(test.param, &cmd_sectrk, sizeof(cmd_sectrk));
    	//ipc_sendmsg(&test,IPC_TOIMG_MSG);
		printf("ImgPixelX = %d\n", m_selcTrak->ImgPixelX);
	printf("ImgPixelY = %d\n", m_selcTrak->ImgPixelY);
	printf("INFO : *********************************\r\n");
	}
	return 0;
}

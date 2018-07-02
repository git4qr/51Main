#include "ipcProc.h"
#include "msg_id.h"
#include <sys/time.h>
#include <assert.h>
#include "CMsgProcess.h"

extern CMsgProcess* sThis;
extern OSA_SemHndl  m_semHndl;

struct timeval ipcProc;
extern selectTrack 	m_selectPara;

CIPCProc::CIPCProc()
{
	pthread_mutex_init(&mutex,NULL);
	CMD_AUTOCHECK fr_img_cmd_autocheck = {0,0,0,0};
	CMD_SENSOR fr_img_cmd_sensor={1};
	CMD_PinP fr_img_cmd_pinp = {0,0,0};
	CMD_TRK fr_img_cmd_trk = {0,0};
	CMD_SECTRK fr_img_cmd_sectrk = {0};
	CMD_ENH fr_img_cmd_enh = {0};
	CMD_MTD fr_img_cmd_mtd = {0};
	CMD_MMT fr_img_cmd_mmt = {0};
	CMD_MMTSELECT fr_img_cmd_mmtsel = {0};
	CMD_TRKDOOR fr_img_cmd_trkdoor = {1};
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
	printf("CIPCProc====>Create!\n");
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
IMGSTATUS * CIPCProc::getAvtStatSharedMem()
{
	ipc_status = ipc_getimgstatus_p();
	assert(ipc_status != NULL);
	return ipc_status;
}
OSDSTATUS *CIPCProc::getOSDSharedMem()
{
	ipc_OSD =ipc_getosdstatus_p();
	assert(ipc_OSD != NULL);
	return ipc_OSD;
}
UTCTRKSTATUS *CIPCProc::getUTCSharedMem()
{
	ipc_UTC = ipc_getutstatus_p();
	assert(ipc_UTC != NULL);
	return ipc_UTC;
}

LKOSDSTATUS* CIPCProc::getLKOSDShareMem()
{
	ipc_LKOSD = ipc_getlkosdstatus_p();
	assert(ipc_LKOSD != NULL);
	return ipc_LKOSD;
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
        		sThis->signalFeedBack(ACK_avtErrorOutput, 0, 0, 0);

        		//gettimeofday(&recv, NULL);
        		//printf("------recv pos------  %d  ,%d \n", recv.tv_sec, recv.tv_usec);
      //  		printf("trackstatus = %d\n", trackstatus);
     //		printf("trackposx = %d, trackposy = %d\n", trackposx, trackposy);

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
	sThis->Change_avtStatus();
	sThis->signalFeedBack(ACK_avtTrkStatus, 0, 0, 0);
		return 0;
}

int  CIPCProc::ipcMutilTargetDetecCtrl(volatile unsigned char ImgMmtStat)//1:open 0:close
{
      memset(test.param,0,PARAMLEN);
	test.cmd_ID = mmt;

	{
		test.param[0]=ImgMmtStat;
	    	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	    	sThis->Change_avtStatus();
	    	sThis->signalFeedBack(ACK_mmtStatus, 0, 0, 0);
	    	//printf("mmt = %d \n", test.param[0]);
	}

		return 0;
}

int  CIPCProc::ipcMutilTargetSelectCtrl(volatile unsigned char ImgMmtSelect)
{
      memset(test.param,0,PARAMLEN);
	test.cmd_ID = mmtselect;
	//if(ImgMmtSelect != fr_img_cmd_mmtsel.ImgMmtSelect)
	{
		test.param[0]=ImgMmtSelect;
	    	//ipc_sendmsg(&test,IPC_TOIMG_MSG);
	}
		return 0;
}

int CIPCProc::IpcMmtLockCtrl(int mmt_Select)
{
	memset(test.param,0,PARAMLEN);
	test.cmd_ID = mmtLock;
	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	sThis->Change_avtStatus();
	sThis->signalFeedBack(ACK_mmtSelectStatus, ACK_mmtSelect_value, mmt_Select, 0);
	return 0;
}

int CIPCProc::ipcImageEnhanceCtrl(volatile unsigned char ImgEnhStat) //1open 0close
{
      memset(test.param,0,PARAMLEN);
	test.cmd_ID = enh;
	{
		test.param[0]=ImgEnhStat;
	    	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	    	sThis->Change_avtStatus();
	    	sThis->signalFeedBack(ACK_EnhStatus, 0, 0, 0);
	}
		return 0;
}

int CIPCProc::ipcMoveTatgetDetecCtrl(volatile unsigned char ImgMtdStat)
{
      memset(test.param,0,PARAMLEN);
	test.cmd_ID = mtd;
	{
		test.param[0]=ImgMtdStat;
	    	ipc_sendmsg(&test,IPC_TOIMG_MSG);
	    	sThis->Change_avtStatus();
	    	sThis->signalFeedBack(ACK_MtdStatus, 0, 0, 0);
	    	printf("MTDStat = %d\n", test.param[0]);

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
	cmd_sectrk.ImgPixelX =m_selcTrak->ImgPixelX;
	cmd_sectrk.ImgPixelY =m_selcTrak->ImgPixelY;
	memcpy(test.param, &cmd_sectrk, sizeof(cmd_sectrk));
    ipc_sendmsg(&test,IPC_TOIMG_MSG);
	sThis->signalFeedBack(ACK_TrkSearchStatus, ACK_TrkSearch_value, m_selcTrak->SecAcqStat, 0);
    	//printf("cmd_sectrk.SecAcqStat = %d\n", cmd_sectrk.SecAcqStat );
		//printf("IPC======>ImgPixelX = %d\n", cmd_sectrk.ImgPixelX );
		//printf("IPC======>ImgPixelY = %d\n", cmd_sectrk.ImgPixelY);
	}
	return 0;
}

int CIPCProc::IpcSensorSwitch(volatile unsigned char ImgSenchannel)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = sensor;
	{
		test.param[0] = ImgSenchannel;
		ipc_sendmsg(&test, IPC_TOIMG_MSG);
		sThis->Change_avtStatus();
		sThis->signalFeedBack(ACK_mainVideoStatus, 0, 0, 0);
		printf("sensorchannel = %d\n", test.param[0]);
	}
	return 0;
}

int CIPCProc::IpcpinpCtrl(volatile unsigned char ImgPipStat)
{
	CMD_PinP cmd_pip;
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = pinp;
	{
		cmd_pip.ImgPicp = ImgPipStat;
		cmd_pip.PicpSensorStat = 1; //*****************************
		cmd_pip.PicpZoomStat = 1;
		memcpy(test.param, &cmd_pip, sizeof(cmd_pip));
		ipc_sendmsg(&test, IPC_TOIMG_MSG);
		sThis->signalFeedBack(ACK_picpStatus, 0, 0, 0);
	}
	return 0;
}

int CIPCProc::IpcAcqDoorCtrl(AcqBoxSize *BoxSize)
{
	memset(test.param, 0, PARAMLEN);
	AcqBoxWH m_acqBoxWH;
	test.cmd_ID = acqBox;
	{
		m_acqBoxWH.AimW = BoxSize->AcqBoxW[0];
		m_acqBoxWH.AimH = BoxSize->AcqBoxH[0];
		printf("IPC======>AimW = %d\n", m_acqBoxWH.AimW);
		printf("IPC======>AimH = %d\n", m_acqBoxWH.AimH);
		memcpy(test.param, &m_acqBoxWH, sizeof(m_acqBoxWH));
		ipc_sendmsg(&test, IPC_TOIMG_MSG);
		sThis->Change_avtStatus();
	}
	return 0;
}

int CIPCProc::IpcIrisAndFocus(char sign)
{
	memset(test.param, 0, PARAMLEN);
	switch(sign)
	{
	case Exit:
		test.cmd_ID = exit_IrisAndFocus;
		break;
	case iris:
		test.cmd_ID = Iris;
		break;
	case Focus:
		test.cmd_ID = focus;
		break;
	}
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcFuncMenu(char sign)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = menu;
	test.param[0] = sign;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	printf("FuncMenu is success\n");
	return 0;
}

int CIPCProc::IpcTrkPosMoveCtrl(POSMOVE * avtMove)
{
	memset(test.param, 0, PARAMLEN);
	CMD_POSMOVE cmd_posMove;
	test.cmd_ID = posmove;
	{
		cmd_posMove.AvtMoveX = avtMove->AvtMoveX;
		cmd_posMove.AvtMoveY = avtMove->AvtMoveY;
		printf("AvtMoveX = %d         ----------      AvtMoveY = %d\r\n", cmd_posMove.AvtMoveX, cmd_posMove.AvtMoveY);
		memcpy(test.param, &cmd_posMove, sizeof(cmd_posMove));
		ipc_sendmsg(&test, IPC_TOIMG_MSG);
		sThis->signalFeedBack(ACK_posMoveStatus, ACK_posMove_value, avtMove->AvtMoveX, avtMove->AvtMoveY);
	}
	return 0;
}

int CIPCProc::IpcConfig()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_config;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	printf("IPCProc=====>send config!\n");
	//gettimeofday(&ipcProc, NULL);
	//printf("-----send config------  %d  ,%d \n", ipcProc.tv_sec, ipcProc.tv_usec);
	return 0;
}

int CIPCProc::IpcConfigOSD()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_osd;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	printf("IPCProc=====>send OSD!\n");
	return 0;
}

int CIPCProc::IpcConfigUTC()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_utctrk;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	printf("IPCProc=====>send UTC!\n");
	return 0;
}

int CIPCProc::IPCConfigCamera()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_camera;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

int CIPCProc::IpcElectronicZoom(int zoom)
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = elecZoom;
	test.param[0] = zoom;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	sThis->signalFeedBack(ACK_ElectronicZoomStatus, ACK_ElectronicZoom_value, zoom, 0);
	return 0;
}

int CIPCProc::IPCChannel_binding(int channel)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}

int CIPCProc::IPCAxisMove(int x, int y)
{
	memset(test.param, 0, PARAMLEN);
	CMD_POSMOVE cmd_axismove;
	test.cmd_ID = axismove;
	cmd_axismove.AvtMoveX = x;
	cmd_axismove.AvtMoveY = y;
	memcpy(test.param, &cmd_axismove, sizeof(cmd_axismove));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	sThis->signalFeedBack(ACK_moveAxisStatus, ACK_moveAxis_value, x, y);
}


int CIPCProc::IPCpicp(int status, int pipChannel)
{
	memset(test.param, 0, PARAMLEN);
	CMD_PinP cmd_pip;
	test.cmd_ID = pinp;
	cmd_pip.ImgPicp = status;
	cmd_pip.PicpSensorStat = pipChannel;
	memcpy(test.param, &cmd_pip, sizeof(cmd_pip));
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	sThis->signalFeedBack(ACK_picpStatus, status, pipChannel, 0);

}

int CIPCProc::IPCswitchVideoChannel(int channel)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}

int CIPCProc::IPCframeCtrl(int fps, int channel)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}

int CIPCProc::IPCcompression_quality(int quality, int channel)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}

int CIPCProc::IPCwordColor(int color)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}

int CIPCProc::IPCwordType(int type)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}

int CIPCProc::IPCwordSize(int size)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}
int CIPCProc::IPCwordDisEnable(int status)
{
	memset(test.param, 0, PARAMLEN);
	return 0;
}

int CIPCProc::IPCLKOSD()
{
	memset(test.param, 0, PARAMLEN);
	test.cmd_ID = read_shm_lkosd;
	ipc_sendmsg(&test, IPC_TOIMG_MSG);
	return 0;
}

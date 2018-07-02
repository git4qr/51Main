#ifndef _IPCPROC_H_
#define _IPCPROC_H_

#include "osa_thr.h"
#include "Ipcctl.h"
#include   "iMessageQUE.h"
#pragma once
class  CIPCProc{

public:
		    CIPCProc();
            ~CIPCProc();
            int Create();
            int Destroy();
		    int  ipcTrackCtrl(volatile unsigned char AvtTrkStat);
		    int  ipcMutilTargetDetecCtrl(volatile unsigned char ImgMmtStat);//1:open 0:close
		    int ipcMutilTargetSelectCtrl(volatile unsigned char ImgMmtSelect);
		    int IpcMmtLockCtrl(int mmt_Select);
		    int  ipcImageEnhanceCtrl(volatile unsigned char ImgEnhStat); //1open 0close
		    int ipcMoveTatgetDetecCtrl(volatile unsigned char ImgMtdStat);
		    int ipcSecTrkCtrl(selectTrack *m_selcTrak);
		    int IpcSensorSwitch(volatile unsigned char ImgSenchannel);
		    int IpcpinpCtrl(volatile unsigned char ImgPipStat);
		    int IpcAcqDoorCtrl(AcqBoxSize *BoxSize);
		    int IpcIrisAndFocus(char sign);
		    int IpcFuncMenu(char sign);
		    int IpcTrkPosMoveCtrl(POSMOVE * avtMove);
		    int IpcElectronicZoom(int zoom);
		    int IpcConfig();
		    int IpcConfigOSD();
		    int IpcConfigUTC();
		    int IPCConfigCamera();

		    int IPCChannel_binding(int channel);
		    int IPCAxisMove(int x, int y);
		    int IPCpicp(int status, int pipChannel);
		    int IPCswitchVideoChannel(int channel);
		    int IPCframeCtrl(int fps, int channel);
		    int IPCcompression_quality(int quality, int channel);
		    int IPCwordColor(int color);
		    int IPCwordType(int type);
		    int IPCwordSize(int size);
		    int IPCwordDisEnable(int status);
		    int IPCLKOSD();

		    IMGSTATUS  *getAvtStatSharedMem();
		    OSDSTATUS *getOSDSharedMem();
		    UTCTRKSTATUS *getUTCSharedMem();
		    LKOSDSTATUS* getLKOSDShareMem();
			  	unsigned int trackstatus;
			  	int trackposx;
			  	int trackposy;
			  	CMD_TRK fr_img_cmd_trk;
			  	CMD_SECTRK fr_img_cmd_sectrk;
			  	CMD_ENH fr_img_cmd_enh;
			  	CMD_MTD fr_img_cmd_mtd;
			  	CMD_MMT fr_img_cmd_mmt;
			  	CMD_MMTSELECT fr_img_cmd_mmtsel;
			  	CMD_TRKDOOR fr_img_cmd_trkdoor;


			    IMGSTATUS *ipc_status;
			    OSDSTATUS *ipc_OSD;
			    UTCTRKSTATUS *ipc_UTC;
			    LKOSDSTATUS 	*ipc_LKOSD;
#if 0
		      static Void  *IPCShamThrdFxn(Void * prm){
		    	  CIPCProc *pThis = (CIPCProc*)prm;
		      	   pThis->getShamdataInThrd();
		      };
#endif
		      static Void   *IPC_childdataRcvn(Void * prm){
		    	  CIPCProc *pThis = (CIPCProc*)prm;
		           pThis->getIPCMsgProc();
		          };
private:
			  	bool exitThreadIPCRcv;
			  	bool exitGetShamDatainThrd;
			     CIPCProc *pThis;
			     OSA_ThrHndl thrHandlPCRcv;
			     OSA_ThrHndl thrHandlPCSham;
			  	pthread_mutex_t mutex;
				SENDST test;
			  	SENDST fr_img_test;
			  	void getIPCMsgProc();
			  	void getShamdataInThrd();

};
#endif

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
		    int  ipcImageEnhanceCtrl(volatile unsigned char ImgEnhStat); //1open 0close
		    int ipcMoveTatgetDetecCtrl(volatile unsigned char ImgMtdStat);
		    int ipcSecTrkCtrl(selectTrack *m_selcTrak);
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

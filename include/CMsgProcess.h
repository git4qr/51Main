#ifndef  _MSGPROCESS_H_
#define  _MSGPROCESS_H_

#pragma once
#include "osa_thr.h"
#include "osa_sem.h"
#include  "PTZ_control.h"
#include "platformControl.h"
#include "iMessageQUE.h"
#include "PTZ_speedTransfer.h"
#include "joystickProcess.h"
#include "UartProcess.h"
#include "NetProcess.h"
#include <iostream>
#include "ipcProc.h"
#include "Ipcctl.h"
#include <opencv2/core/core.hpp>
//#include "glut.h"

using namespace cv;
using namespace std;

typedef struct _main_thr_obj{
	OSA_ThrHndl	 thrHandleProc;
	volatile bool	 exitProcThread;
	bool			 initFlag;
	void 		 *pParent;
}MAIN_ProcThrObj;


typedef struct {
		int  m_TrkStat;
		int  m_SenSwStat;
		int m_ZoomLongStat;
		int m_ZoomShortStat;
		int m_TrkBoxSizeStat;
		int m_SecTrkStat;
		int m_IrisUpStat;
		int m_IrisDownStat;
		int m_FoucusFarStat;
		int m_FoucusNearStat;
		int m_PresetStat;
		int m_ImgEnhanceStat;
		int m_MmtStat;
		int m_MmtSelectNum;
		volatile unsigned char m_AimPosXStat;
		volatile unsigned char m_AimPosYStat;
		int m_AxisXStat;
		int m_AxisYStat;
}CurrParaStat,*PCurrParaStat;

typedef struct {
	void (*keyboardfunc)(unsigned char key, int x, int y);
}ms_init_prm;

class CMsgProcess{
        public:
             CMsgProcess();
              ~CMsgProcess();
              int Create();
              int Destroy();
               int Init();
		      int Run();
		      void MSGAPI_ExtInputCtrl_ZoomLong();
		      void MSGAPI_ExtInputCtrl_ZoomShort();
		      void MSGAPI_ExtInputCtrl_FocusFar();
		      void MSGAPI_ExtInputCtrlFocusNear();
		      void MSGAPI_ExtInputCtrl_IrisUp();
		      void MSGAPI_ExtInputCtrl_IrisDown();
		      void MSGAPI_ExtInputCtrl_Preset();
		      void MSGAPI_ExtInputCtrl_AXIS();
		      void MSGAPI_IPCInputCtrl_Axis();
		      int GetExtIputCtrlValue(int msg);
		  	 void realtime_avtStatus();
		  	 void Change_avtStatus();
		  	 void signalFeedBack(int signal, int index, int value, int s_value);
             CMsgProcess *pThis;
         	 CJosStick *m_jos;
         	 CUartProcess *m_uart;
         	 CPTZControl *m_ptz;
         	 CIPCProc  *m_ipc;
         	 CNetWork *mNet;
        public:
         	char m_SetRealTime, m_CurRealTime;
         	bool m_ChangeRealTime;
         	typedef struct {
         		int val;
         	}test;
         	test aa;
         	test *ptest;
         	void modifierAVTProfile(int block, int field, float value, PlatformCtrl_CreateParams *m_cfgPrm, configPlatParam_InitParams *m_cfgPprm, HPLTCTRL  cfg_plt);
         	int updataProfile();
         	int answerRead(int block, int field);
        	PlatformCtrl_CreateParams m_pltParams;
        	configPlatParam_InitParams m_cfgPlatParam;
        	HPLTCTRL   m_plt;
        private:
         	float *cfg_value;
private:
	MAIN_ProcThrObj	  mainProcThrObj;
	//OSA_MutexHndl    m_mutex;
	void main_proc_func();
	int MAIN_threadCreate(void);
	int Config_threadCreate();
	int MAIN_threadDestroy(void);
	void main_thread_proc();
	void processMsg(int msg);
	void PlantTrackerInputPara(void);
	int configAvtFromFile();

//	int MSGAPI_initial();
//static 	void app_PlantCtrl(long lParam );
 protected:
	ms_init_prm init_prm;
	volatile int m_value;
	CPTZSpeedTransfer  m_ptzSpeed;
	PlatformFilter_InitParams m_cfgPlatFilter;
	PLATFORMCTRL_TrackerInput m_pltInput;
	PLATFORMCTRL_Output m_pltOutput;
	static void keyboard_event(unsigned char key, int x, int y);

	static void *mainProcTsk(void *context)
	{
		//OSA_waitMsecs(200);
		OSA_printf("%s:%d context=%p\n", __func__, __LINE__, context);
		MAIN_ProcThrObj  * pObj= (MAIN_ProcThrObj*) context;
		CMsgProcess *ctxHdl = (CMsgProcess *) pObj->pParent;
		ctxHdl->main_thread_proc();

		return NULL;
	}
#if 0
    OSA_ThrHndl m_thrRead;
    static void *readCfgParamFunc(void * context){
    	CMsgProcess *user = (CMsgProcess *) context;
    	user->configAvtFromFile();
    	return NULL;
    }
#endif
};





#endif

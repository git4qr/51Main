/********************************************************************************
  Copyright (C), 2011-2012, CamRun Tech. Co., Ltd.
  File name:    msgProssDef.h
  Author:   xavier       Version:  1.0.0      Date: 2013-04-17
  Description: this file define the globle message process api function.
                and message id , both form the external interface and internal
                interface.
  Others:
  Function List:
  History:
    1. Date:
       Author:
       Modification:
    2. ...
********************************************************************************/

#ifndef _MSG_PROSS_DEF_H_
#define _MSG_PROSS_DEF_H_

//#ifdef __cplusplus
//extern "C" {
//#endif
#include "app_global.h"
#include "msgDriv.h"

/*
Cmd_Mesg_TrkCtrl=1, 	//1
Cmd_Mesg_SensorCtrl,		//2
Cmd_Mesg_Unable,//3
Cmd_Mesg_Unable2,//4
Cmd_Mesg_TrkBoxCtrl,		//5
Cmd_Mesg_TrkSearch,		//6
Cmd_Mesg_IrisUp,			//7
Cmd_Mesg_IrisDown,	//8
Cmd_Mesg_FocusUp,	//9
Cmd_Mesg_FocusDown, 	//10
Cmd_Mesg_ImgEnh,				//11
Cmd_Mesg_Auto_Iris_Focus,		//12
Cmd_Mesg_AIMPOS_X,					//pov
Cmd_Mesg_AIMPOS_Y,					//pov
Cmd_Mesg_Zoom,				//throttle
*/

typedef enum _sys_msg_id_ {
    MSGID_SYS_INIT  = 0,           ///< internal cmd system init.
    MSGID_SYS_RESET,
    MSGID_EXT_INPUT_TRACKCTRL,
    MSGID_EXT_INPUT_MTDCTRL,
    MSGID_EXT_INPUT_OPTICZOOMLONGCTRL,
    MSGID_EXT_INPUT_OPTICZOOMSHORTCTRL,
    MSGID_EXT_INPUT_TRCKBOXSIZECTRL,
    MSGID_EXT_INPUT_TRACKSEARCHCTRL,
    MSGID_EXT_INPUT_IRISUPCTRL,
    MSGID_EXT_INPUT_IRISDOWNCTRL,
    MSGID_EXT_INPUT_FOCUSFARCHCTRL,
    MSGID_EXT_INPUT_FOCUSNEARCTRL,
    MSGID_EXT_INPUT_IMGENHCTRL,
    MSGID_EXT_INPUT_MMTCRTL,
    MSGID_EXT_INPUT_AIMPOSXCTRL,
    MSGID_EXT_INPUT_AIMPOSYCTRL,
    MSGID_EXT_INPUT_PLATCTRL,
    MSGID_EXT_INPUT_SwitchSensor,
    MSGID_EXT_INPUT_PresetCtrl,
    MSGID_EXT_INPUT_configWrite,

    MSGID_IPC_INPUT_TRACKCTRL,
    MSGID_IPC_INPUT_MTDCTRL,
    MSGID_IPC_INPUT_MMTCRTL,
    MSGID_IPC_INPUT_IMGENHCTRL,
    MSGID_IPC_INPUT_TRCKBOXSIZECTRL,
    MSGID_IPC_Config,
    MSGID_IPC_OSD,
    MSGID_IPC_UTC,
    MSGID_IPC_Camera
}eSysMsgId, MSG_PROC_ID;

int  MSGAPI_initial();
//void MSGAPI_initial(MSGDRIV_Handle handle);
void MSGAPI_unInitial(MSGDRIV_Handle handle);
//void MSGAPI_init_device(LPARAM lParam       /*=NULL*/);
void usd_MSGAPI_ExtInpuCtrl_Track(long p);
void usd_MSGAPI_ExtInpuCtrl_Mtd(long p);
void usd_MSGAPI_ExtInpuCtrl_TrkBoxSize(long p);
void usd_MSGAPI_ExtInpuCtrl_TrkSearch(long p);
void usd_MSGAPI_ExtInpuCtrl_IrisUp(long p);
void usd_MSGAPI_ExtInpuCtrl_IrisDwon(long p);
void usd_MSGAPI_ExtInpuCtrl_FocusFar(long p);
void usd_MSGAPI_ExtInpuCtrl_FocusNear(long p);
void usd_MSGAPI_ExtInpuCtrl_ImgEnh(long p);
void usd_MSGAPI_ExtInpuCtrl_Mmt(long p);
void MSGAPI_ExtInputCtrl_MmtSelect(int msg);
void usd_MSGAPI_ExtInpuCtrl_AIMPOSX(long p);
void usd_MSGAPI_ExtInpuCtrl_AIMPOSY(long p);
void usd_MSGAPI_ExtInpuCtrl_ZoomLong(long p);
void usd_MSGAPI_ExtInpuCtrl_AXISX(long p);
void usd_MSGAPI_ExtInpuCtrl_AXISY(long p);
void usd_MSGAPI_ExtInpuCtrl_AXIS(long p);
void usd_MSGAPI_EXTINPUTCtrl_Sensor(long p);
void usd_MSGAPI_EXTINPUTCtrl_Preset(long p);
void usd_MSGAPI_IPCProfile(long p);
void usd_MSGAPI_IPCConfigWrite(long p);
void usd_MSGAPI_IPCReadOSD(long p);
void usd_MSGAPI_IPCReadUTC(long p);
void usd_MSGAPI_IPCReadCamera(long p);
void  MSGAPI_StatusConvertFunc(int msg);




//#ifdef __cplusplus
//}
//#endif

#endif

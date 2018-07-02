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

typedef enum _sys_msg_id_ {
    MSGID_SYS_INIT  = 0,           ///< internal cmd system init.
    MSGID_SYS_RESET,
    MSGID_EXT_INPUT_TRACKCTRL,
    MSGID_EXT_INPUT_MTDCTRL,
    MSGID_EXT_INPUT_OPTICZOOMLONGCTRL,
    MSGID_EXT_INPUT_OPTICZOOMSHORTCTRL,
    MSGID_EXT_INPUT_IrisAndFocusAndExit,
    MSGID_EXT_INPUT_TRACKSEARCHCTRL,
    MSGID_EXT_INPUT_FuncMenu,
    MSGID_EXT_INPUT_IRISDOWNCTRL,
    MSGID_EXT_INPUT_FOCUSFARCHCTRL,
    MSGID_EXT_INPUT_FOCUSNEARCTRL,
    MSGID_EXT_INPUT_IMGENHCTRL,
    MSGID_EXT_INPUT_MMTCRTL,
    MSGID_EXT_INPUT_MMTSelect,
    MSGID_EXT_INPUT_AIMPOSXCTRL,
    MSGID_EXT_INPUT_AIMPOSYCTRL,
    MSGID_EXT_INPUT_PLATCTRL,
    MSGID_EXT_INPUT_SwitchSensor,
    MSGID_EXT_INPUT_PresetCtrl,
    MSGID_EXT_INPUT_configWrite,
    MSGID_EXT_INPUT_configWrite_Save,
    MSGID_EXT_INPUT_config_Read,
    MSGID_EXT_INPUT_kboard,
    MSGID_EXT_INPUT_OSD,

    MSGID_IPC_INPUT_TRACKCTRL,
    MSGID_IPC_INPUT_MTDCTRL,
    MSGID_IPC_INPUT_MMTCRTL,
    MSGID_IPC_INPUT_IMGENHCTRL,
    MSGID_IPC_INPUT_TRCKBOXSIZECTRL,
    MSGID_IPC_MainElectronicZoom,
    MSGID_IPC_Config,
    MSGID_IPC_OSD,
    MSGID_IPC_UTC,
    MSGID_IPC_Camera,
    MSGID_IPC_Channel_binding,
    MSGID_IPC_AxisMove,
    MSGID_IPC_saveAxis,
    MSGID_IPC_picp,
    MSGID_IPC_switchVideoChannel,
    MSGID_IPC_frameCtrl,
    MSGID_IPC_compression_quality,
    MSGID_IPC_wordColor,
    MSGID_IPC_wordType,
    MSGID_IPC_wordSize,
    MSGID_IPC_wordDisEnable,

}eSysMsgId, MSG_PROC_ID;

int  MSGAPI_initial();
//void MSGAPI_initial(MSGDRIV_Handle handle);
void MSGAPI_unInitial(MSGDRIV_Handle handle);
//void MSGAPI_init_device(LPARAM lParam       /*=NULL*/);
void usd_MSGAPI_ExtInpuCtrl_Track(long p);
void usd_MSGAPI_ExtInpuCtrl_Mtd(long p);
void usd_MSGAPI_ExtInpuCtrl_IrisAndFocusAndExit(long p);
void usd_MSGAPI_ExtInpuCtrl_TrkSearch(long p);
void usd_MSGAPI_ExtInpuCtrl_FuncMenu(long p);
void usd_MSGAPI_ExtInpuCtrl_IrisDwon(long p);
void usd_MSGAPI_ExtInpuCtrl_FocusFar(long p);
void usd_MSGAPI_ExtInpuCtrl_FocusNear(long p);
void usd_MSGAPI_ExtInpuCtrl_ImgEnh(long p);
void usd_MSGAPI_ExtInpuCtrl_Mmt(long p);
void usd_MSGAPI_ExtInputCtrl_MmtSelect( long p);
void usd_MSGAPI_ExtInpuCtrl_AIMPOSX(long p);
void usd_MSGAPI_ExtInpuCtrl_AIMPOSY(long p);
void usd_MSGAPI_ExtInpuCtrl_ZoomLong(long p);
void usd_MSGAPI_ExtInpuCtrl_AXISX(long p);
void usd_MSGAPI_ExtInpuCtrl_AXISY(long p);
void usd_MSGAPI_ExtInpuCtrl_AXIS(long p);
void usd_MSGAPI_EXTINPUTCtrl_Sensor(long p);
void usd_MSGAPI_EXTINPUTCtrl_Preset(long p);
void usd_MSGAPI_EXTINPUT_config_Read(long p);
void usd_MSGAPI_EXTINPUT_kboard(long p);
void usd_MSGAPI_EXTINPUT_LKOSD(long p);

void usd_MSGAPI_IPCProfile(long p);
void usd_MSGAPI_IPCConfigWrite(long p);
void usd_MSGAPI_IPCConfigWrite_Save(long p);
void usd_MSGAPI_IPCReadOSD(long p);
void usd_MSGAPI_IPCReadUTC(long p);
void usd_MSGAPI_IPCReadCamera(long p);
void usd_MSGAPI_IPCElectronicZoom(long p);

void usd_MSGAPI_IPCChannel_binding(long p);
void usd_MSGAPI_IPCAxisMove(long p);
void usd_MSGAPI_IPCsaveAxis(long p);
void usd_MSGAPI_IPCpicp(long p);
void usd_MSGAPI_IPCswitchVideoChannel(long p);
void usd_MSGAPI_IPCframeCtrl(long p);
void usd_MSGAPI_IPCcompression_quality(long p);
void usd_MSGAPI_IPCwordColor(long p);
void usd_MSGAPI_IPCwordType(long p);
void usd_MSGAPI_IPCwordSize(long p);
void usd_MSGAPI_IPCwordDisEnable(long p);

void  MSGAPI_StatusConvertFunc(int msg);
void MSGAPI_MmtLock();




//#ifdef __cplusplus
//}
//#endif

#endif

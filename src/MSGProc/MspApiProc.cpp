#include <sys/select.h>
#include  <sys/time.h>
#include <stdlib.h>
#include "msgProssDef.h"
#include "CMsgProcess.h"
#include "msg_id.h"
#include "Ipcctl.h"
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

extern CMsgProcess* sThis;
extern CurrParaStat  m_CurrStat;
extern  selectTrack 	m_selectPara;
extern POSMOVE	m_avtMove;
static  bool   shieldInitTrkSearchBit= false;
static bool cfg_stat = false;
static int m_valuex;
static int m_valuey;
char  m_AvtTrkAimSize;
char m_eSenserStat;
void  MSGAPI_StatusConvertFunc(int msg)
{
	sThis->m_ipc->ipc_status = sThis->m_ipc->getsharedmemstat();

	switch(msg){
	case Cmd_Mesg_TrkCtrl:
		sThis->m_jos->EXT_Ctrl[msg - 1] = !(sThis->m_ipc->ipc_status->AvtTrkStat);
		printf("AvtStatus = %d\n", sThis->m_ipc->ipc_status->AvtTrkStat);
		break;
	case Cmd_Mesg_Mtd:
		sThis->m_jos->EXT_Ctrl[msg - 1] = !(sThis->m_ipc->ipc_status->MtdState[0]);
		break;
	case Cmd_Mesg_TrkBoxCtrl:
		m_AvtTrkAimSize = (sThis->m_ipc->ipc_status->AvtTrkAimSize +1)%Trk_SizeMax;
		sThis->m_jos->EXT_Ctrl[msg - 1] = m_AvtTrkAimSize;
		break;
	case Cmd_Mesg_ImgEnh:
		sThis->m_jos->EXT_Ctrl[msg - 1] = !(sThis->m_ipc->ipc_status->ImgEnhStat[0]);
		break;
	case Cmd_Mesg_Mmt:
		sThis->m_jos->EXT_Ctrl[msg - 1] = !(sThis->m_ipc->ipc_status->MmtStat[0]);
		break;
	case Cmd_Mesg_SensorCtrl:
		m_eSenserStat =  (sThis->m_ipc->ipc_status->SensorStat +1)%eSen_Max;
		sThis->m_jos->EXT_Ctrl[msg - 1] = m_eSenserStat;
		break;
	default:
		break;
	}

}
void usd_MSGAPI_ExtInpuCtrl_Track(long p)
{
	MSGAPI_StatusConvertFunc(Cmd_Mesg_TrkCtrl);
	sThis->m_ipc->ipcTrackCtrl((unsigned char)sThis->m_jos->EXT_Ctrl[Cmd_Mesg_TrkCtrl - 1]);
}

void usd_MSGAPI_ExtInpuCtrl_Mtd(long p)
{
	MSGAPI_StatusConvertFunc(Cmd_Mesg_Mtd);
	sThis->m_ipc->ipcMoveTatgetDetecCtrl((unsigned char)sThis->m_jos->EXT_Ctrl[Cmd_Mesg_Mtd - 1]);
}

void usd_MSGAPI_ExtInpuCtrl_ZoomLong(long p)
{
	m_CurrStat.m_ZoomLongStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_ZoomLong);
	sThis->MSGAPI_ExtInputCtrl_ZoomLong();
}
void usd_MSGAPI_ExtInpuCtrl_ZoomShort(long p)
{
	m_CurrStat.m_ZoomShortStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_ZoomShort);
	sThis->MSGAPI_ExtInputCtrl_ZoomShort();
}

void usd_MSGAPI_ExtInpuCtrl_TrkBoxSize(long p)
{
	MSGAPI_StatusConvertFunc(Cmd_Mesg_TrkBoxCtrl);
	sThis->m_ipc->IpcTrkDoorCtrl((unsigned char) sThis->m_jos->EXT_Ctrl[Cmd_Mesg_TrkBoxCtrl - 1]);
}


void usd_MSGAPI_ExtInpuCtrl_TrkSearch(long p)
{
	sThis->m_ipc->ipc_status = sThis->m_ipc->getsharedmemstat();
	if(shieldInitTrkSearchBit){
	m_CurrStat.m_SecTrkStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_TrkSearch);
	if(sThis->m_ipc->ipc_status->AvtTrkStat){
				if (m_CurrStat.m_SecTrkStat==0){
						m_CurrStat.m_AxisXStat = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISX);
						m_CurrStat.m_AxisYStat = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISY);
					 if( m_CurrStat.m_AxisXStat>=0)
							       m_valuex =min(32640,m_CurrStat.m_AxisXStat);
					 else  m_valuex =max(-32640,m_CurrStat.m_AxisXStat);
					 if( m_CurrStat.m_AxisYStat>=0)
							      m_valuey =min(32400,m_CurrStat.m_AxisYStat);
				     else  m_valuey =max(-32400,m_CurrStat.m_AxisYStat);
						  m_selectPara.SecAcqStat=m_CurrStat.m_SecTrkStat;
						  m_selectPara.ImgPixelX = sThis->m_jos->JosToWinX(m_valuex);
						  m_selectPara.ImgPixelY=	sThis->m_jos->JosToWinY(m_valuey);
						  sThis->m_ipc->ipcSecTrkCtrl(&m_selectPara);

				}
	   }
	else {
		if (m_CurrStat.m_SecTrkStat==0){
							m_CurrStat.m_AxisXStat = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISX);
							m_CurrStat.m_AxisYStat = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISY);
						 if( m_CurrStat.m_AxisXStat>=0)
								       m_valuex =min(32640,m_CurrStat.m_AxisXStat);
						 else  m_valuex =max(-32640,m_CurrStat.m_AxisXStat);
						 if( m_CurrStat.m_AxisYStat>=0)
								      m_valuey =min(32400,m_CurrStat.m_AxisYStat);
					     else  m_valuey =max(-32400,m_CurrStat.m_AxisYStat);
							  m_selectPara.SecAcqStat=m_CurrStat.m_SecTrkStat;
							  m_selectPara.ImgPixelX = sThis->m_jos->JosToWinX(m_valuex);
							  m_selectPara.ImgPixelY=	sThis->m_jos->JosToWinY(m_valuey);
							  sThis->m_ipc->ipcSecTrkCtrl(&m_selectPara);
					}
        	}
	}
	shieldInitTrkSearchBit =true;
}
void usd_MSGAPI_ExtInpuCtrl_IrisUp(long p)
{
	m_CurrStat.m_IrisUpStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_IrisUp);
	sThis->MSGAPI_ExtInputCtrl_IrisUp();
}
void usd_MSGAPI_ExtInpuCtrl_IrisDwon(long p)
{
	m_CurrStat.m_IrisDownStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_IrisDown);
	sThis->MSGAPI_ExtInputCtrl_IrisDown();
}
void usd_MSGAPI_ExtInpuCtrl_FocusFar(long p)
{
	m_CurrStat.m_FoucusFarStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_FocusFar);
	sThis->MSGAPI_ExtInputCtrl_FocusFar();
}
void usd_MSGAPI_ExtInpuCtrl_FocusNear(long p)
{
	 m_CurrStat.m_FoucusNearStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_FocusNear);
	sThis->MSGAPI_ExtInputCtrlFocusNear();
}
void usd_MSGAPI_ExtInpuCtrl_ImgEnh(long p)
{
	MSGAPI_StatusConvertFunc(Cmd_Mesg_ImgEnh);
	sThis->m_ipc->ipcImageEnhanceCtrl((unsigned char) sThis->m_jos->EXT_Ctrl[Cmd_Mesg_ImgEnh - 1]);
}
void usd_MSGAPI_ExtInpuCtrl_Mmt(long p)
{
	MSGAPI_StatusConvertFunc(Cmd_Mesg_Mmt);
	sThis->m_ipc->ipcMutilTargetDetecCtrl((unsigned char)sThis->m_jos->EXT_Ctrl[Cmd_Mesg_Mmt - 1]);
}

void MSGAPI_ExtInputCtrl_MmtSelect(int msg)
{
	m_CurrStat.m_MmtSelectNum = msg;
	sThis->m_ipc->ipcMutilTargetSelectCtrl((unsigned char) m_CurrStat.m_MmtSelectNum);
}

void usd_MSGAPI_ExtInpuCtrl_AIMPOSX(long p)
{
	m_CurrStat.m_AimPosXStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_AIMPOS_X);
	m_avtMove.AvtMoveX = m_CurrStat.m_AimPosXStat;
	sThis->m_ipc->IpcTrkPosMoveCtrl(&m_avtMove);
}

void usd_MSGAPI_ExtInpuCtrl_AIMPOSY(long p)
{
	m_CurrStat.m_AimPosYStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_AIMPOS_Y);
	m_avtMove.AvtMoveY = m_CurrStat.m_AimPosYStat;
	sThis->m_ipc->IpcTrkPosMoveCtrl(&m_avtMove);
}

void usd_MSGAPI_EXTINPUTCtrl_Sensor(long p)
{
	MSGAPI_StatusConvertFunc(Cmd_Mesg_SensorCtrl);
	sThis->m_ipc->IpcSensorSwitch((unsigned char)sThis->m_jos->EXT_Ctrl[Cmd_Mesg_SensorCtrl - 1]);
}

void usd_MSGAPI_EXTINPUTCtrl_Preset(long p)
{
	m_CurrStat.m_PresetStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_PresetCtrl);
		sThis->MSGAPI_ExtInputCtrl_Preset();
}

void usd_MSGAPI_IPCProfile(long p)
{
	//if(cfg_stat)
	sThis->m_ipc->IpcConfig();
	//cfg_stat = true;
}

void usd_MSGAPI_IPCConfigWrite(long p)
{
	int block = sThis->m_uart->Host_Ctrl[config_Wblock];
	int field = sThis->m_uart->Host_Ctrl[config_Wfield];
	float value = sThis->m_uart->Host_Ctrl[config_Wvalue];
	printf("block = %d, field = %d, value = %f\n", block, field, value);
	sThis->modifierAVTProfile(block, field, value);
}

void usd_MSGAPI_IPCReadOSD(long p)
{
	printf("MspApiProc======>CROSS_AXIS_HEIGHT = %d\n",sThis->m_ipc->ipc_OSD->CROSS_AXIS_HEIGHT);
	printf("MspApiProc======>CROSS_AXIS_WIDTH = %d\n",sThis->m_ipc->ipc_OSD->CROSS_AXIS_WIDTH);
	sThis->m_ipc->IpcConfigOSD();
}

void usd_MSGAPI_IPCReadUTC(long p)
{
	sThis->m_ipc->IpcConfigUTC();
}

void usd_MSGAPI_IPCReadCamera(long p)
{
	sThis->m_ipc->IPCConfigCamera();
}

void usd_MSGAPI_IPCElectronicZoom(long p)
{
	sThis->m_ipc->IpcElectronicZoom(sThis->m_uart->Host_Ctrl[ElectronicZoom]);
}

void usd_MSGAPI_IPCChannel_binding(long p)
{
	int channel = sThis->m_uart->Host_Ctrl[Channel_binding];
	sThis->m_ipc->IPCChannel_binding(channel);
}

void usd_MSGAPI_IPCAxisMove(long p)
{
	int x = sThis->m_uart->Host_Ctrl[moveAxisX];
	int y = sThis->m_uart->Host_Ctrl[moveAxisY];
	sThis->m_ipc->IPCAxisMove(x, y);
}

void usd_MSGAPI_IPCsaveAxis(long p)
{
	int x = sThis->m_ipc->ipc_status->opticAxisPosX[0];
	int y = sThis->m_ipc->ipc_status->opticAxisPosY[0];

}

void usd_MSGAPI_IPCpicp(long p)
{
	int status = sThis->m_uart->Host_Ctrl[picp];
	int channel = sThis->m_uart->Host_Ctrl[picpChannel];
	sThis->m_ipc->IPCpicp(status, channel);
}

void usd_MSGAPI_IPCswitchVideoChannel(long p)
{
	int channel = sThis->m_uart->Host_Ctrl[switchVideoChannel];
	sThis->m_ipc->IPCswitchVideoChannel(channel);
}

void usd_MSGAPI_IPCframeCtrl(long p)
{

}

void usd_MSGAPI_IPCcompression_quality(long p)
{

}

void usd_MSGAPI_IPCwordColor(long p)
{

}

void usd_MSGAPI_IPCwordType(long p)
{

}

void usd_MSGAPI_IPCwordSize(long p)
{

}

void usd_MSGAPI_IPCwordDisEnable(long p)
{

}

void usd_MSGAPI_EXTINPUT_config_Read(long p)
{

}

void usd_MSGAPI_EXTINPUT_kboard(long p)
{

}

void usd_MSGAPI_ExtInpuCtrl_AXIS(long p)
{
	m_CurrStat.m_AxisXStat = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISX);
	m_CurrStat.m_AxisYStat = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISY);
	sThis->MSGAPI_ExtInputCtrl_AXIS();
	     if (m_CurrStat.m_SecTrkStat){
	          if( m_CurrStat.m_AxisXStat>=0)
	        	    m_valuex =min(32640,m_CurrStat.m_AxisXStat);
	        	    else  m_valuex =max(-32640,m_CurrStat.m_AxisXStat);
	          if( m_CurrStat.m_AxisYStat>=0)
	        	    m_valuey =min(32400,m_CurrStat.m_AxisYStat);
	        	    else  m_valuey =max(-32400,m_CurrStat.m_AxisYStat);
		      m_selectPara.SecAcqStat=m_CurrStat.m_SecTrkStat;
		      m_selectPara.ImgPixelX = sThis->m_jos->JosToWinX(m_valuex);
		      m_selectPara.ImgPixelY=	sThis->m_jos->JosToWinY(m_valuey);
		      sThis->m_ipc->ipcSecTrkCtrl(&m_selectPara);
	         }
}

int  MSGAPI_initial()
{
   MSGDRIV_Handle handle=&g_MsgDrvObj;
    assert(handle != NULL);
    memset(handle->msgTab, 0, sizeof(MSGTAB_Class) * MAX_MSG_NUM);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_TRACKCTRL,usd_MSGAPI_ExtInpuCtrl_Track,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_MTDCTRL,usd_MSGAPI_ExtInpuCtrl_Mtd,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_TRCKBOXSIZECTRL,usd_MSGAPI_ExtInpuCtrl_TrkBoxSize,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_TRACKSEARCHCTRL,usd_MSGAPI_ExtInpuCtrl_TrkSearch,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IRISUPCTRL,usd_MSGAPI_ExtInpuCtrl_IrisUp,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IRISDOWNCTRL,usd_MSGAPI_ExtInpuCtrl_IrisDwon,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_FOCUSFARCHCTRL,usd_MSGAPI_ExtInpuCtrl_FocusFar,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_FOCUSNEARCTRL,usd_MSGAPI_ExtInpuCtrl_FocusNear,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IMGENHCTRL,usd_MSGAPI_ExtInpuCtrl_ImgEnh,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_MMTCRTL,usd_MSGAPI_ExtInpuCtrl_Mmt,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_AIMPOSXCTRL,usd_MSGAPI_ExtInpuCtrl_AIMPOSX,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_AIMPOSYCTRL,usd_MSGAPI_ExtInpuCtrl_AIMPOSY,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_OPTICZOOMLONGCTRL,usd_MSGAPI_ExtInpuCtrl_ZoomLong,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_OPTICZOOMSHORTCTRL,usd_MSGAPI_ExtInpuCtrl_ZoomShort,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_PLATCTRL,usd_MSGAPI_ExtInpuCtrl_AXIS,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_SwitchSensor,usd_MSGAPI_EXTINPUTCtrl_Sensor,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_PresetCtrl,usd_MSGAPI_EXTINPUTCtrl_Preset,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_INPUT_TRACKCTRL,usd_MSGAPI_ExtInpuCtrl_AXIS,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_Config,usd_MSGAPI_IPCProfile,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_configWrite,usd_MSGAPI_IPCConfigWrite, 0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_OSD,usd_MSGAPI_IPCReadOSD ,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_UTC,usd_MSGAPI_IPCReadUTC,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_Camera,usd_MSGAPI_IPCReadCamera, 0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_MainElectronicZoom, usd_MSGAPI_IPCElectronicZoom, 0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_Channel_binding,usd_MSGAPI_IPCChannel_binding,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_AxisMove,usd_MSGAPI_IPCAxisMove,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_saveAxis,usd_MSGAPI_IPCsaveAxis,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_picp,usd_MSGAPI_IPCpicp,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_switchVideoChannel,usd_MSGAPI_IPCswitchVideoChannel,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_frameCtrl,usd_MSGAPI_IPCframeCtrl,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_compression_quality,usd_MSGAPI_IPCcompression_quality,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_wordColor,usd_MSGAPI_IPCwordColor,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_wordType,usd_MSGAPI_IPCwordType,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_wordSize,usd_MSGAPI_IPCwordSize,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_wordDisEnable,usd_MSGAPI_IPCwordDisEnable,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_config_Read,usd_MSGAPI_EXTINPUT_config_Read,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_kboard,usd_MSGAPI_EXTINPUT_kboard,0);

    return 0;
}

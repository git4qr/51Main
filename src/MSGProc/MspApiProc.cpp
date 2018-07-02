#include <sys/select.h>
#include  <sys/time.h>
#include <stdlib.h>
#include "msgProssDef.h"
#include "CMsgProcess.h"
#include "msg_id.h"
#include "Ipcctl.h"

extern CMsgProcess* sThis;
extern CurrParaStat  m_CurrStat;
extern  selectTrack 	m_selectPara;
extern POSMOVE	m_avtMove;
extern AcqBoxSize	m_acqBox_Size;
static  bool   shieldInitTrkSearchBit= false;
static bool cfg_stat = false;
static int m_valuex;
static int m_valuey;
char  m_AvtTrkAimSize;
char m_eSenserStat;
char iRet = -1;
int acqAIM[5][5] = {{10,10}, {20,20}, {30,30}, {40,40}, {50,50}};
char pip_channel;
void  MSGAPI_StatusConvertFunc(int msg)
{
	sThis->m_ipc->ipc_status = sThis->m_ipc->getAvtStatSharedMem();
	switch(msg){
	case Cmd_Mesg_TrkCtrl:
		sThis->m_jos->EXT_Ctrl[msg - 1] = !(sThis->m_ipc->ipc_status->AvtTrkStat);
		break;
	case Cmd_Mesg_Mtd:
		sThis->m_jos->EXT_Ctrl[msg - 1] = !(sThis->m_ipc->ipc_status->MtdState[0]);
		break;
		/*
	case Cmd_Mesg_AcqBoxCtrl:
		iRet = (iRet +1)%Trk_SizeMax;
		m_acqBox_Size.AcqBoxW[0] = acqAIM[iRet][0];
		m_acqBox_Size.AcqBoxH[0] = acqAIM[iRet][1];
		break;
		*/
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
	case Cmd_Mesg_Picp:
		pip_channel = sThis->m_uart->Host_Ctrl[picpChannel];
		sThis->m_uart->Host_Ctrl[picp] = !(sThis->m_ipc->ipc_status->ImgPicp[pip_channel]);
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

void usd_MSGAPI_ExtInpuCtrl_IrisAndFocusAndExit(long p)
{
	//MSGAPI_StatusConvertFunc(Cmd_Mesg_IrisAndFocusAndExit);
	char sign = sThis->GetExtIputCtrlValue(Cmd_Mesg_IrisAndFocusAndExit);
	int up_down = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISY);
	sThis->m_ipc->IpcIrisAndFocus(sign);
	switch(sign)
	{
	case iris:
			sThis->MSGAPI_ExtInputCtrl_Iris(up_down);
		break;
	case Focus:
		sThis->MSGAPI_ExtInputCtrl_Focus(up_down);
		break;
	}

}


void usd_MSGAPI_ExtInpuCtrl_TrkSearch(long p)
{
	sThis->m_ipc->ipc_status = sThis->m_ipc->getAvtStatSharedMem();
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
void usd_MSGAPI_ExtInpuCtrl_FuncMenu(long p)
{
	int sign = sThis->GetExtIputCtrlValue(Cmd_Mesg_FuncMenu);
	sThis->m_ipc->IpcFuncMenu(sign);
	//m_CurrStat.m_IrisUpStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_IrisUp);
	//sThis->MSGAPI_ExtInputCtrl_IrisUp();
}
void usd_MSGAPI_ExtInpuCtrl_IrisDwon(long p)
{
	m_CurrStat.m_IrisDownStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_IrisDown);
	sThis->MSGAPI_ExtInputCtrl_IrisDown();
}
void usd_MSGAPI_ExtInpuCtrl_FocusFar(long p)
{
	m_CurrStat.m_FoucusFarStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_FocusFar);
	//sThis->MSGAPI_ExtInputCtrl_FocusFar();
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

void usd_MSGAPI_ExtInputCtrl_MmtSelect(long p)
{
	p = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISX);  //X Select Aim
	sThis->m_ipc->ipcMutilTargetSelectCtrl((unsigned char) p);
}

void MSGAPI_MmtLock()
{
	 int mmt_value = sThis->GetExtIputCtrlValue(Cmd_Mesg_AXISX);
	sThis->m_ipc->IpcMmtLockCtrl(mmt_value);
}
void usd_MSGAPI_ExtInpuCtrl_AIMPOSX(long p)
{
	m_avtMove.AvtMoveX = sThis->GetExtIputCtrlValue(Cmd_Mesg_AIMPOS_X);
	sThis->m_ipc->IpcTrkPosMoveCtrl(&m_avtMove);
	m_avtMove.AvtMoveX = 0;
}

void usd_MSGAPI_ExtInpuCtrl_AIMPOSY(long p)
{
	m_avtMove.AvtMoveY = sThis->GetExtIputCtrlValue(Cmd_Mesg_AIMPOS_Y);
	sThis->m_ipc->IpcTrkPosMoveCtrl(&m_avtMove);
	m_avtMove.AvtMoveY = 0;
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
	sThis->modifierAVTProfile(block, field, value, &(sThis->m_pltParams),&(sThis->m_cfgPlatParam), sThis->m_plt);
	sThis->signalFeedBack(ACK_config_Write, ACK_config_Wblock, block, field);
}

void usd_MSGAPI_IPCConfigWrite_Save(long p)
{
	sThis->updataProfile();
}

void usd_MSGAPI_IPCReadOSD(long p)
{
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
	float x = float(sThis->m_ipc->ipc_status->opticAxisPosX[0]);
	float y = float(sThis->m_ipc->ipc_status->opticAxisPosY[0]);
	sThis->modifierAVTProfile(24, 0, x, NULL, NULL, NULL);
	sThis->modifierAVTProfile(25, 0, y, NULL, NULL, NULL);
	sThis->updataProfile();
}

void usd_MSGAPI_IPCpicp(long p)
{
	MSGAPI_StatusConvertFunc(Cmd_Mesg_Picp);
	int status = sThis->m_uart->Host_Ctrl[picp];
	//int channel = sThis->m_uart->Host_Ctrl[picpChannel];
	sThis->m_ipc->IPCpicp(status, pip_channel);
}

void usd_MSGAPI_IPCswitchVideoChannel(long p)
{
	int channel = sThis->m_uart->Host_Ctrl[switchVideoChannel];
	sThis->m_ipc->IPCswitchVideoChannel(channel);
}

void usd_MSGAPI_IPCframeCtrl(long p)
{
	int fps = sThis->m_uart->Host_Ctrl[frameCtrl];
	int channel = sThis->m_uart->Host_Ctrl[frameChannel];
	sThis->m_ipc->IPCframeCtrl(fps, channel);
}

void usd_MSGAPI_IPCcompression_quality(long p)
{
	int quality = sThis->m_uart->Host_Ctrl[compression_quality];
	int channel = sThis->m_uart->Host_Ctrl[compressionChannel];
	sThis->m_ipc->IPCcompression_quality(quality, channel);
}

void usd_MSGAPI_IPCwordColor(long p)
{
	int color = sThis->m_uart->Host_Ctrl[wordColor];
	sThis->m_ipc->IPCwordColor(color);
}

void usd_MSGAPI_IPCwordType(long p)
{
	int type = sThis->m_uart->Host_Ctrl[wordType];
	sThis->m_ipc->IPCwordType(type);
}

void usd_MSGAPI_IPCwordSize(long p)
{
	int size = sThis->m_uart->Host_Ctrl[wordSize];
	sThis->m_ipc->IPCwordSize(size);
}

void usd_MSGAPI_IPCwordDisEnable(long p)
{
	int status = sThis->m_uart->Host_Ctrl[wordDisEnable];
	sThis->m_ipc->IPCwordDisEnable(status);
}

void usd_MSGAPI_EXTINPUT_config_Read(long p)
{
	int block = sThis->m_uart->Host_Ctrl[config_Rblock];
	int field = sThis->m_uart->Host_Ctrl[config_Rfield];
	sThis->answerRead( block,  field);
	sThis->signalFeedBack(ACK_config_Reab, ACK_config_Rblock, block, field);
}

void usd_MSGAPI_EXTINPUT_kboard(long p)
{

}

void usd_MSGAPI_EXTINPUT_LKOSD(long p)
{
	sThis->m_ipc->ipc_LKOSD = sThis->m_ipc->getLKOSDShareMem();
	memcpy(sThis->m_ipc->ipc_LKOSD, &(sThis->m_uart->osd_Param), sizeof(sThis->m_uart->osd_Param));

    printf("MspApiProce IPC====> speedinfo = %d\n", sThis->m_ipc->ipc_LKOSD->speedInfo);
    printf("MspApiProce  IPC====> distance = %d\n", sThis->m_ipc->ipc_LKOSD->distanceInfo);
    printf("MspApiProce  IPC====> speed = %d\n", sThis->m_ipc->ipc_LKOSD->speed);

    printf("MspApiProce  OSD====> speed = %d\n", sThis->m_uart->osd_Param.speed);
    printf("MspApiProce OSD====> speedinfo = %d\n", sThis->m_uart->osd_Param.speedInfo);

    printf("MspApiProce  OSD====> distance = %d\n", sThis->m_uart->osd_Param.distanceInfo);
	sThis->m_ipc->IPCLKOSD();
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
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IrisAndFocusAndExit,usd_MSGAPI_ExtInpuCtrl_IrisAndFocusAndExit,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_TRACKSEARCHCTRL,usd_MSGAPI_ExtInpuCtrl_TrkSearch,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_FuncMenu,usd_MSGAPI_ExtInpuCtrl_FuncMenu,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IRISDOWNCTRL,usd_MSGAPI_ExtInpuCtrl_IrisDwon,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_FOCUSFARCHCTRL,usd_MSGAPI_ExtInpuCtrl_FocusFar,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_FOCUSNEARCTRL,usd_MSGAPI_ExtInpuCtrl_FocusNear,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IMGENHCTRL,usd_MSGAPI_ExtInpuCtrl_ImgEnh,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_MMTCRTL,usd_MSGAPI_ExtInpuCtrl_Mmt,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_MMTSelect,usd_MSGAPI_ExtInputCtrl_MmtSelect,0);
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
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_configWrite_Save, usd_MSGAPI_IPCConfigWrite_Save, 0);
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
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_OSD, usd_MSGAPI_EXTINPUT_LKOSD, 0);

    return 0;
}

#include <sys/select.h>
#include  <sys/time.h>
#include <stdlib.h>
#include "msgProssDef.h"
#include "joystickProcess.h"
#include "CMsgProcess.h"
#include "msg_id.h"
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

static int m_valuex;
static int m_valuey;

void usd_MSGAPI_ExtInpuCtrl_Track(long p)
{
	m_CurrStat.m_TrkStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_TrkCtrl);
	sThis->m_ipc->ipcTrackCtrl((unsigned char)m_CurrStat.m_TrkStat);
}

void usd_MSGAPI_ExtInpuCtrl_SwichSenr(long p)
{
	m_CurrStat.m_TrkStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_SensorCtrl);
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
	m_CurrStat.m_TrkBoxSizeStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_TrkBoxCtrl);
}


void usd_MSGAPI_ExtInpuCtrl_TrkSearch(long p)
{
	m_CurrStat.m_SecTrkStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_TrkSearch);
	if(m_CurrStat.m_TrkStat){
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
							   sThis->m_jos->JOS_Value[0]=1;
							   sThis->m_ipc->ipcTrackCtrl(1);
					}

	}
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
	m_CurrStat.m_ImgEnhanceStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_ImgEnh);
	sThis->m_ipc->ipcImageEnhanceCtrl((unsigned char)m_CurrStat.m_ImgEnhanceStat);
}
void usd_MSGAPI_ExtInpuCtrl_unable(long p)
{
	sThis->MSGAPI_ExtInputCtrl_unable();
}

void usd_MSGAPI_ExtInpuCtrl_AIMPOSX(long p)
{
	m_CurrStat.m_AimPosXStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_AIMPOS_X);
}

void usd_MSGAPI_ExtInpuCtrl_AIMPOSY(long p)
{
	m_CurrStat.m_AimPosYStat=sThis->GetExtIputCtrlValue(Cmd_Mesg_AIMPOS_Y);
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
void usd_MSGAPI_IPCInpuCtrl_AXIS(long p)
{
	sThis->MSGAPI_IPCInputCtrl_Axis();
}
int  MSGAPI_initial()
{
   MSGDRIV_Handle handle=&g_MsgDrvObj;
    assert(handle != NULL);
    memset(handle->msgTab, 0, sizeof(MSGTAB_Class) * MAX_MSG_NUM);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_TRACKCTRL,usd_MSGAPI_ExtInpuCtrl_Track,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_SENSWITCHCTRL,usd_MSGAPI_ExtInpuCtrl_SwichSenr,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_TRCKBOXSIZECTRL,usd_MSGAPI_ExtInpuCtrl_TrkBoxSize,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_TRACKSEARCHCTRL,usd_MSGAPI_ExtInpuCtrl_TrkSearch,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IRISUPCTRL,usd_MSGAPI_ExtInpuCtrl_IrisUp,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IRISDOWNCTRL,usd_MSGAPI_ExtInpuCtrl_IrisDwon,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_FOCUSFARCHCTRL,usd_MSGAPI_ExtInpuCtrl_FocusFar,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_FOCUSNEARCTRL,usd_MSGAPI_ExtInpuCtrl_FocusNear,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_IMGENHCTRL,usd_MSGAPI_ExtInpuCtrl_ImgEnh,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_unable,usd_MSGAPI_ExtInpuCtrl_unable,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_AIMPOSXCTRL,usd_MSGAPI_ExtInpuCtrl_AIMPOSX,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_AIMPOSYCTRL,usd_MSGAPI_ExtInpuCtrl_AIMPOSY,0);
     MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_OPTICZOOMLONGCTRL,usd_MSGAPI_ExtInpuCtrl_ZoomLong,0);
     MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_OPTICZOOMSHORTCTRL,usd_MSGAPI_ExtInpuCtrl_ZoomShort,0);
    MSGDRIV_attachMsgFun(handle,MSGID_EXT_INPUT_PLATCTRL,usd_MSGAPI_ExtInpuCtrl_AXIS,0);

    //MSGDRIV_attachMsgFun(handle,MSGID_IPC_INPUT_TRACKCTRL,usd_MSGAPI_IPCInpuCtrl_AXIS,0);
    MSGDRIV_attachMsgFun(handle,MSGID_IPC_INPUT_TRACKCTRL,usd_MSGAPI_ExtInpuCtrl_AXIS,0);
    return 0;
}

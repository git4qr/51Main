#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include "CMsgProcess.h"
#include  "msg_id.h"
#include "app_status.h"
#include "iMessageQUE.h"
#include "msgDriv.h"
#include "msgProssDef.h"
CMsgProcess* sThis =NULL;
CurrParaStat  m_CurrStat;
selectTrack 	m_selectPara;
CMsgProcess::CMsgProcess()
{
	m_plt = NULL;
	m_ptz = NULL;
	m_jos = NULL;
	m_ipc=NULL;
	pThis = this;
	memset(&mainProcThrObj, 0, sizeof(MAIN_ProcThrObj));
	memset(&m_ptzSpeed,0,sizeof(m_ptzSpeed));
	memset(&m_CurrStat, 0 ,sizeof(m_CurrStat));
	memset(&m_selectPara,0,sizeof(m_selectPara));
	PlatformCtrl_CreateParams_Init(&m_pltParams);
	sThis = this;
}

CMsgProcess::~CMsgProcess()
{
		Destroy();
}

void CMsgProcess::PlantTrackerInputPara(void)
{
	memset(&m_pltInput, 0, sizeof(m_pltInput));
	m_pltInput.fBoresightPositionX = 1920/2;
	m_pltInput.fBoresightPositionY = 1080/2;
	m_pltInput.fAcqWindowPositionX = m_pltInput.fBoresightPositionX;
	m_pltInput.fAcqWindowPositionY = m_pltInput.fBoresightPositionY;
	m_pltInput.fAcqWindowSizeX = 40;
	m_pltInput.fAcqWindowSizeY = 30;
	m_pltInput.fTargetSizeX = m_pltInput.fAcqWindowSizeX;
	m_pltInput.fTargetSizeY = m_pltInput.fAcqWindowSizeY;
}

int CMsgProcess::Create()
{
	int iRet = 0;
	MSGDRIV_create();
	iRet = Work_queCreate();
	if (iRet	){
		fprintf(stdout,"Create que error!!\r\n");
	}
	PlantTrackerInputPara();
	OSA_assert(m_plt == NULL);
	m_plt = PlatformCtrl_Create(&m_pltParams);
	OSA_assert(m_plt != NULL);
	m_ptz = new CPTZControl();
	m_ptz->Create();
   //Jos Create
	m_jos = new CJosStick();
	m_jos->Create();
  // IPC communication
    m_ipc = new CIPCProc();
	m_ipc->Create();
	// Create main thread
	MAIN_threadCreate();	
	return iRet;
}

int CMsgProcess::Destroy()
{
	OSA_assert(m_jos != NULL);
	m_jos->Destroy();
	m_jos = NULL;
	OSA_printf("INFO: Destroy Jos ctrl ok");
	OSA_assert(m_ipc!= NULL);
	m_ipc->Destroy();
	OSA_printf("INFO: Destroy IPC ctrl ok");
	OSA_assert(m_plt != NULL);
	PlatformCtrl_Delete(m_plt);
	//m_bCreate = false;
	OSA_printf("%s ... ok", __func__);
	m_plt = NULL;
	OSA_assert(m_plt != NULL);
	return 0;
}
int CMsgProcess::Init()
{
	MSGAPI_initial();
	m_jos->Init();
	return 0 ;
	
}

int CMsgProcess::Run() 
{	
	m_jos->Run();
	return 0;

}

int CMsgProcess::MAIN_threadCreate(void)
{
	int iRet = MES_SOK;
	mainProcThrObj.exitProcThread = false;
	mainProcThrObj.initFlag = true;
	mainProcThrObj.pParent = (void*)this;
	iRet = OSA_thrCreate(&mainProcThrObj.thrHandleProc, mainProcTsk, 0, 0, &mainProcThrObj);
	return iRet;
}

void CMsgProcess::processMsg(int msg)
{
					switch(msg){
						case Cmd_Mesg_TrkCtrl:
						MSGDRIV_send(MSGID_EXT_INPUT_TRACKCTRL,0);
							 break;
						case Cmd_Mesg_SensorCtrl:
						MSGDRIV_send(MSGID_EXT_INPUT_SENSWITCHCTRL,0);
							break;
						case Cmd_Mesg_ZoomLong:
						MSGDRIV_send(MSGID_EXT_INPUT_OPTICZOOMLONGCTRL,0);
						break;
						case Cmd_Mesg_ZoomShort:
						MSGDRIV_send(MSGID_EXT_INPUT_OPTICZOOMSHORTCTRL,0);
						break;
						case Cmd_Mesg_TrkBoxCtrl:
						MSGDRIV_send(MSGID_EXT_INPUT_TRCKBOXSIZECTRL,0);
							break;
						case Cmd_Mesg_TrkSearch:
							MSGDRIV_send(MSGID_EXT_INPUT_TRACKSEARCHCTRL,0);
							break;
						case Cmd_Mesg_IrisUp:
							MSGDRIV_send(MSGID_EXT_INPUT_IRISUPCTRL,0);
								break;
						case Cmd_Mesg_IrisDown:
							MSGDRIV_send(MSGID_EXT_INPUT_IRISDOWNCTRL,0);
							break;
						case Cmd_Mesg_FocusFar:
							MSGDRIV_send(MSGID_EXT_INPUT_FOCUSFARCHCTRL,0);
							break;
						case Cmd_Mesg_FocusNear:
							MSGDRIV_send(MSGID_EXT_INPUT_FOCUSNEARCTRL,0);
							break;
						case Cmd_Mesg_ImgEnh:
					 MSGDRIV_send(MSGID_EXT_INPUT_IMGENHCTRL,0);
							break;
						case Cmd_Mesg_unable:
						MSGDRIV_send(MSGID_EXT_INPUT_unable,0);
							break;
						case Cmd_Mesg_AIMPOS_X:
							MSGDRIV_send(MSGID_EXT_INPUT_AIMPOSXCTRL,0);
							break;
						case Cmd_Mesg_AIMPOS_Y:
							MSGDRIV_send(MSGID_EXT_INPUT_AIMPOSYCTRL,0);
						  break;
						case Cmd_Mesg_AXISX:
						   MSGDRIV_send(MSGID_EXT_INPUT_PLATCTRL,0);
							break;
						case Cmd_Mesg_AXISY:
							MSGDRIV_send(MSGID_EXT_INPUT_PLATCTRL,0);
						    break;
					case   Cmd_IPC_TrkCtrl:
							MSGDRIV_send(MSGID_IPC_INPUT_TRACKCTRL,0);
						break;
						default:
							fprintf(stdout,"INFO: can not excute here\r\n");
					}
}

void CMsgProcess::main_thread_proc()
{
	    int msg;
		OSA_printf("%s: Main Proc Tsk Is Entering..[%d].\n",__func__, mainProcThrObj.exitProcThread);
		while(!mainProcThrObj.exitProcThread){
					Work_queGet(&msg);
					processMsg(msg);
		}
}

int CMsgProcess::GetExtIputCtrlValue(int msg)
{
	int *pstr = m_jos->JOS_Value;
	return *(pstr + msg -1);
}

void CMsgProcess::MSGAPI_ExtInputCtrl_ZoomLong()
{
	if(m_CurrStat.m_ZoomLongStat)
		m_ptz->m_iSetZoomSpeed = -1;
	else
		m_ptz->m_iSetZoomSpeed = 0;
	m_ptz->Move();
}
void CMsgProcess::MSGAPI_ExtInputCtrl_ZoomShort()
{
	if(m_CurrStat.m_ZoomShortStat)
		m_ptz->m_iSetZoomSpeed = 1;
	else
		m_ptz->m_iSetZoomSpeed = 0;
	m_ptz->Move();
}

void CMsgProcess::MSGAPI_ExtInputCtrl_unable()  //预留
{

}

void CMsgProcess::MSGAPI_ExtInputCtrl_FocusFar()
{

	if(m_CurrStat.m_FoucusFarStat)
		m_ptz->m_iSetFocusFarSpeed = -1;
	else
		m_ptz->m_iSetFocusFarSpeed = 0;
	m_ptz->Move();
}

void CMsgProcess::MSGAPI_ExtInputCtrlFocusNear()
{
	if(m_CurrStat.m_FoucusNearStat)
		m_ptz->m_iSetFocusNearSpeed = 1;
	else
		m_ptz->m_iSetFocusNearSpeed = 0;
	m_ptz->Move();
}

void CMsgProcess::MSGAPI_ExtInputCtrl_IrisUp()
{

	if(m_CurrStat.m_IrisUpStat)
		m_ptz->m_iSetIrisSpeed = -1;
	else
		m_ptz->m_iSetIrisSpeed = 0;
	m_ptz->Move();
}

void CMsgProcess::MSGAPI_ExtInputCtrl_IrisDown()
{

	if(m_CurrStat.m_IrisDownStat)
		m_ptz->m_iSetIrisSpeed = 1;
	else
		m_ptz->m_iSetIrisSpeed = 0;
	m_ptz->Move();
}

void CMsgProcess::MSGAPI_IPCInputCtrl_Axis()
{

	m_pltInput.iTrkAlgState=this->m_ipc->trackstatus + 1;
	m_pltInput.fTargetBoresightErrorX=this->m_ipc->trackposx;
	m_pltInput.fTargetBoresightErrorY=this->m_ipc->trackposy;
	PlatformCtrl_TrackerInput(m_plt, &m_pltInput);
	PlatformCtrl_TrackerOutput(m_plt, &m_pltOutput);
	if(m_ptz != NULL){
		m_ptz->m_iSetPanSpeed = m_ptzSpeed.GetPanSpeed((int)m_pltOutput.fPlatformDemandX);
		m_ptz->m_iSetTiltSpeed = m_ptzSpeed.GetTiltSpeed((int)m_pltOutput.fPlatformDemandY);
		m_ptz->Move();
		}

}

void CMsgProcess::MSGAPI_ExtInputCtrl_AXIS()
{
	 if(!m_CurrStat.m_TrkStat &&! m_CurrStat.m_SecTrkStat ){
	    m_pltInput.iTrkAlgState= m_CurrStat.m_TrkStat + 1;
		PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickXInput, m_CurrStat.m_AxisXStat/32760.f);
		PlatformCtrl_VirtualInput(m_plt, DevUsr_AimpointRefineXInput, m_CurrStat.m_AxisXStat/32760.f);
		PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickYInput, m_CurrStat.m_AxisYStat/32760.f);
		PlatformCtrl_VirtualInput(m_plt, DevUsr_AimpointRefineYInput, m_CurrStat.m_AxisYStat/32760.f);
		m_pltInput.fTargetBoresightErrorX=(float)0;
		m_pltInput.fTargetBoresightErrorY=(float)0;
	  }
	 else{
		       m_pltInput.iTrkAlgState= this->m_ipc->trackstatus + 1;
			  m_pltInput.fTargetBoresightErrorX=this->m_ipc->trackposx;
			  m_pltInput.fTargetBoresightErrorY=this->m_ipc->trackposx;
		}


		PlatformCtrl_TrackerInput(m_plt, &m_pltInput);
		PlatformCtrl_TrackerOutput(m_plt, &m_pltOutput);
		if(m_ptz != NULL){
			m_ptz->m_iSetPanSpeed = m_ptzSpeed.GetPanSpeed((int)m_pltOutput.fPlatformDemandX);
			m_ptz->m_iSetTiltSpeed = m_ptzSpeed.GetTiltSpeed((int)m_pltOutput.fPlatformDemandY);
			m_ptz->Move();
			}
}


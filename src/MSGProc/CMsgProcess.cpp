#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include "CMsgProcess.h"
#include  "msg_id.h"
#include "iMessageQUE.h"
#include "msgDriv.h"
#include "msgProssDef.h"
#include <sys/time.h>
#include "Ipcctl.h"
struct timeval ptz111;
CMsgProcess* sThis =NULL;
CurrParaStat  m_CurrStat;
selectTrack 	m_selectPara;
POSMOVE 		m_avtMove;
bool  InitSystem  = false;

CMsgProcess::CMsgProcess()
{
	m_plt = NULL;
	m_ptz = NULL;
	m_jos = NULL;
	m_uart = NULL;
	m_ipc=NULL;
	ptest = &aa;
	pThis = this;
	sThis = this;
	cfg_value = new int[700];
	m_SetRealTime = m_CurRealTime = 0;
	m_ChangeRealTime = false;
	memset(cfg_value, 0, sizeof(cfg_value));
	//Config_threadCreate();
	memset(&mainProcThrObj, 0, sizeof(MAIN_ProcThrObj));
	memset(&m_ptzSpeed,0,sizeof(m_ptzSpeed));
	memset(&m_CurrStat, 0 ,sizeof(m_CurrStat));
	memset(&m_selectPara,0,sizeof(m_selectPara));
	memset(&m_avtMove, 0, sizeof(m_avtMove));

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
	  // IPC communication
	 m_ipc = new CIPCProc();
      m_ipc->Create();
	configAvtFromFile();
	PlatformCtrl_CreateParams_Init(&m_pltParams, &m_cfgPlatParam);
	int iRet = 0;
	MSGDRIV_create();
	iRet = Work_queCreate();
	if (iRet	){
		fprintf(stdout,"Create que error!!\r\n");
	}
	//OSA_semCreate(m_semHndl, 1, 0);
	PlantTrackerInputPara();
	OSA_assert(m_plt == NULL);
	m_plt = PlatformCtrl_Create(&m_pltParams);
	OSA_assert(m_plt != NULL);
	m_ptz = new CPTZControl();
   m_ptz->Create();
   //Jos Create
	m_jos = new CJosStick();
	m_jos->Create();
	//uart Create
	m_uart = new CUartProcess();
	//m_uart->Create();
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
	OSA_assert(m_uart != NULL);
	m_uart = NULL;
	OSA_assert(m_ipc!= NULL);
	m_ipc->Destroy();
	OSA_printf("INFO: Destroy IPC ctrl ok");
	OSA_assert(m_plt != NULL);
	PlatformCtrl_Delete(m_plt);
	//m_bCreate = false;
	OSA_printf("%s ... ok", __func__);
	m_plt = NULL;
	OSA_assert(m_plt != NULL);
	delete cfg_value;
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

int CMsgProcess::Config_threadCreate()
{
	int iRet;
	iRet = OSA_thrCreate(&m_thrRead, readCfgParamFunc , 0, 0, (void*)this);
		if(iRet != 0)
	fprintf(stdout,"config creat failed\n");
		return 0;
}
void CMsgProcess::processMsg(int msg)
{

	sThis->m_ipc->ipc_status = sThis->m_ipc->getsharedmemstat();

					switch(msg){
						case Cmd_Mesg_TrkCtrl:
							if(sThis->m_ipc->ipc_status->MmtStat[0])
								MSGAPI_ExtInputCtrl_MmtSelect(1);
							else
								MSGDRIV_send(MSGID_EXT_INPUT_TRACKCTRL,0);
							 break;
						case Cmd_Mesg_Mtd:
							if(sThis->m_ipc->ipc_status->MmtStat[0])
							MSGAPI_ExtInputCtrl_MmtSelect(2);
							else
						MSGDRIV_send(MSGID_EXT_INPUT_MTDCTRL,0);
							break;
						case Cmd_Mesg_ZoomLong:
							if(sThis->m_ipc->ipc_status->MmtStat[0])
							MSGAPI_ExtInputCtrl_MmtSelect(3);
							else
						MSGDRIV_send(MSGID_EXT_INPUT_OPTICZOOMLONGCTRL,0);
						break;
						case Cmd_Mesg_ZoomShort:
							if(sThis->m_ipc->ipc_status->MmtStat[0])
							MSGAPI_ExtInputCtrl_MmtSelect(4);
							else
						MSGDRIV_send(MSGID_EXT_INPUT_OPTICZOOMSHORTCTRL,0);
						break;
						case Cmd_Mesg_TrkBoxCtrl:
							if(sThis->m_ipc->ipc_status->MmtStat[0])
							MSGAPI_ExtInputCtrl_MmtSelect(5);
							else
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
						case Cmd_Mesg_Mmt:
						MSGDRIV_send(MSGID_EXT_INPUT_MMTCRTL,0);
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
						case Cmd_Mesg_SensorCtrl:
							MSGDRIV_send(MSGID_EXT_INPUT_SwitchSensor,0);
							break;
						case Cmd_Mesg_PresetCtrl:
							MSGDRIV_send(MSGID_EXT_INPUT_PresetCtrl,0);
							break;
					case   Cmd_IPC_TrkCtrl:
							MSGDRIV_send(MSGID_IPC_INPUT_TRACKCTRL,0);
						break;
						default:
							fprintf(stdout,"INFO: can not excute here\r\n");
					}
}

int  CMsgProcess::configAvtFromFile()
{
	string cfgAvtFile;
	int configId_Max = 256;
	char  cfg_avt[20] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt");

	if(fp != NULL){
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		if(len < 10)
			return  -1;
		else{
			FileStorage fr(cfgAvtFile, FileStorage::READ);
			if(fr.isOpened()){
#if 1
				for(int i=0; i<configId_Max; i++){
									sprintf(cfg_avt, "cfg_avt_%d", i);
									cfg_value[i] = (int)fr[cfg_avt];
									printf(" update cfg [%d] %d \n", i, cfg_value[i]);
								}
#endif
				typedef struct{
					string UartDev;
					int BaudRate;
					char DataBits;
					char ParityBits;
					string StopBits;
					int FlowCtrl;
				}uratParam;
				{   // plat
				int m_plat;
				m_plat = 2;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.scalarX = (float)fr[cfg_avt];

				m_plat = 3;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.scalarY = (float)fr[cfg_avt];

				m_plat = 4;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.demandMaxX = (float)fr[cfg_avt];

				m_plat = 5;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.demandMinX = (float)fr[cfg_avt];

				m_plat = 6;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.demandMaxY = (float)fr[cfg_avt];

				m_plat = 7;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.demandMinY = (float)fr[cfg_avt];

				m_plat = 12;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.bleedUsed = (int)fr[cfg_avt];
				printf("CMsgProcess===> bleedUsed = %d\n", m_cfgPlatParam.bleedUsed);

				m_plat = 13;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.bleedX = (float)fr[cfg_avt];
				printf("CMsgProcess===> bleedX = %f\n", m_cfgPlatParam.bleedX);

				m_plat = 14;
				sprintf(cfg_avt, "cfg_avt_%d", m_plat);
				m_cfgPlatParam.bleedY = (float)fr[cfg_avt];
				printf("CMsgProcess===> bleedY = %f\n", m_cfgPlatParam.bleedY);
				} //   plat





				{//pid
					int m_pid;
					m_pid = 17;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.P0 = (float)fr[cfg_avt];

					m_pid = 18;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.P1 = (float)fr[cfg_avt];

					m_pid = 19;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.P2 = (float)fr[cfg_avt];

					m_pid = 20;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.L1 = (float)fr[cfg_avt];

					m_pid = 21;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.L2 = (float)fr[cfg_avt];

					m_pid = 22;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.G = (float)fr[cfg_avt];
					printf("CMsgProcess===> G = %f\n", m_cfgPlatParam.m__cfg_platformFilterParam.G);

					m_pid = 23;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.P02 = (float)fr[cfg_avt];

					m_pid = 24;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.P12 = (float)fr[cfg_avt];

					m_pid = 25;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.P22 = (float)fr[cfg_avt];

					m_pid = 26;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.L12 = (float)fr[cfg_avt];

					m_pid = 27;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.L22 = (float)fr[cfg_avt];

					m_pid = 28;
					sprintf(cfg_avt, "cfg_avt_%d", m_pid);
					m_cfgPlatParam.m__cfg_platformFilterParam.G2 = (float)fr[cfg_avt];

				}//pid




				{ // osd
				int m_osd;
				m_osd =226;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				ptest->val = (int)fr[cfg_avt];
				fprintf(stdout, "CMsgProcess=====>AVT_ 226: = %d\n", ptest->val );
#if 0
				R = 192;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->MAIN_Sensor = (int)fr[cfg_avt];
				R = 193;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->Timedisp_9 = (int)fr[cfg_avt];
				R = 194;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->OSD_text_show = (int)fr[cfg_avt];
				R = 195;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->OSD_text_color = (int)fr[cfg_avt];
				R = 196;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->OSD_text_alpha = (int)fr[cfg_avt];
				R = 197;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->OSD_text_font = (int)fr[cfg_avt];
				R = 198;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->OSD_text_size = (int)fr[cfg_avt];
				R = 199;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->OSD_draw_show = (int)fr[cfg_avt];
				R = 200;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->OSD_draw_color = (int)fr[cfg_avt];
				R = 201;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->CROSS_AXIS_WIDTH = (int)fr[cfg_avt];
				R = 202;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->CROSS_AXIS_HEIGHT = (int)fr[cfg_avt];
				R = 203;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->Picp_CROSS_AXIS_WIDTH = (int)fr[cfg_avt];
				R = 204;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->Picp_CROSS_AXIS_HEIGHT = (int)fr[cfg_avt];
				R = 205;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch0_acqRect_width = (int)fr[cfg_avt];
				R = 206;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch1_acqRect_width = (int)fr[cfg_avt];
				R = 207;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch2_acqRect_width = (int)fr[cfg_avt];
				R = 208;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch3_acqRect_width = (int)fr[cfg_avt];
				R = 209;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch4_acqRect_width = (int)fr[cfg_avt];
				R = 210;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch5_acqRect_width = (int)fr[cfg_avt];
				R = 211;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch0_acqRect_height = (int)fr[cfg_avt];
				R = 212;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch1_acqRect_height = (int)fr[cfg_avt];
				R = 213;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch2_acqRect_height = (int)fr[cfg_avt];
				R = 214;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch3_acqRect_height = (int)fr[cfg_avt];
				R = 215;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch4_acqRect_height = (int)fr[cfg_avt];
				R = 216;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->ch5_acqRect_height = (int)fr[cfg_avt];
				R = 217;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen1_width = (int)fr[cfg_avt];
				R = 218;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen2_width = (int)fr[cfg_avt];
				R = 219;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen3_width = (int)fr[cfg_avt];
				R = 220;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen4_width = (int)fr[cfg_avt];
				R = 221;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen5_width = (int)fr[cfg_avt];
				R = 222;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen1_height = (int)fr[cfg_avt];
				R = 223;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen2_height = (int)fr[cfg_avt];
				R = 224;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen3_height = (int)fr[cfg_avt];
				R = 225;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen4_height = (int)fr[cfg_avt];
				R = 226;
				sprintf(cfg_avt, "cfg_avt_%d", m_osd);
				m_ipc->ipc_OSD->bomen5_height = (int)fr[cfg_avt];
#endif
				}//osd
							}else
								return -1;
			}
		}
#if 0
	string cfgCameraFile;
		int cfgId_Max = 671;
		char  cfg_camera[10] = "cfg_avt_";
		cfgCameraFile = "camera_Profile.yml";
		FILE *fp_camera = fopen(cfgCameraFile.c_str(), "rt");

		if(fp_camera != NULL){
			fseek(fp_camera, 0, SEEK_END);
			int len = ftell(fp_camera);
			fclose(fp_camera);
			if(len < 10)
				return  -1;
			else{
				FileStorage fr(cfgCameraFile, FileStorage::READ);
				if(fr.isOpened()){
					for(int j=256; j<cfgId_Max;j++){
										sprintf(cfg_camera, "cfg_avt_%d", j);
										cfg_value[j] = (int )fr[cfg_camera];
										printf(" update cfg [%d] %d \n", j, cfg_value[j]);
									}
								}else
									return -1;
				}
			}
#endif
	}

void CMsgProcess::modifierAVTProfile(int block, int field, int value)
{
	int check = ((block -1) * 16 + field);
	switch(check)
	{
		case 22:
			m_cfgPlatParam.m__cfg_platformFilterParam.G = value;
		break;
		case 28:
			m_cfgPlatParam.m__cfg_platformFilterParam.G2 = value;
			break;
	}
}

int CMsgProcess::updataProfile()
{
	string cfgAvtFile;
	int configId_Max = 256;
	char  cfg_avt[10] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt+");
	int test[256];

	if(fp != NULL){
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		if(len < 10)
			return  -1;
		else{
			FileStorage fr(cfgAvtFile, FileStorage::WRITE);
			if(fr.isOpened()){

				for(int i=0; i<configId_Max; i++){
									sprintf(cfg_avt, "cfg_avt_%d", i);
									test[i] = 0;
									fr[cfg_avt] << test[i];
				}
			}else
				return -1;
		}
	}
}

void CMsgProcess::realtime_avtStatus()
{
	m_ChangeRealTime = (m_CurRealTime != m_SetRealTime);
	m_CurRealTime = m_SetRealTime;
	if(m_ChangeRealTime){
		memcpy(&(sThis->m_uart->avt_status), sThis->m_ipc->ipc_status, sizeof(sThis->m_uart->avt_status));
	}
	m_ChangeRealTime = false;
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
	int *pstr = m_jos->EXT_Ctrl;
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

void CMsgProcess::MSGAPI_ExtInputCtrl_Preset()
{
	if(m_CurrStat.m_PresetStat)
		m_ptz->m_iSetPreset = 1;
	else
		m_ptz->m_iSetPreset = 0;
	m_ptz->Move();
	printf("preset Move!!!\n");
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
	sThis->m_ipc->ipc_status = sThis->m_ipc->getsharedmemstat();
	 if( ! m_CurrStat.m_SecTrkStat && sThis->m_ipc->ipc_status->AvtTrkStat == 0){
	    m_pltInput.iTrkAlgState= m_CurrStat.m_TrkStat + 1;
		PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickXInput, m_CurrStat.m_AxisXStat/32760.f);
		PlatformCtrl_VirtualInput(m_plt, DevUsr_AimpointRefineXInput, m_CurrStat.m_AxisXStat/32760.f);
		PlatformCtrl_VirtualInput(m_plt, DevUsr_AcqJoystickYInput, m_CurrStat.m_AxisYStat/32760.f);
		PlatformCtrl_VirtualInput(m_plt, DevUsr_AimpointRefineYInput, m_CurrStat.m_AxisYStat/32760.f);
		m_pltInput.fTargetBoresightErrorX=(float)0;
		m_pltInput.fTargetBoresightErrorY=(float)0;
	  }
	 else{
		       m_pltInput.iTrkAlgState= sThis->m_ipc->ipc_status->AvtTrkStat + 1;
			  m_pltInput.fTargetBoresightErrorX=this->m_ipc->trackposx;
			  m_pltInput.fTargetBoresightErrorY=this->m_ipc->trackposy;
		}


		PlatformCtrl_TrackerInput(m_plt, &m_pltInput);
		PlatformCtrl_TrackerOutput(m_plt, &m_pltOutput);
		if(m_ptz != NULL){
		//	gettimeofday(&ptz111, NULL);
		//	printf("recv ptz++++++++ %d \n", ptz111.tv_usec);
			m_ptz->m_iSetPanSpeed = m_ptzSpeed.GetPanSpeed((int)m_pltOutput.fPlatformDemandX);
			m_ptz->m_iSetTiltSpeed = m_ptzSpeed.GetTiltSpeed((int)m_pltOutput.fPlatformDemandY);
			m_ptz->Move();
			sThis->m_uart->errorOutPut[0] = this->m_ipc->trackposx;
			sThis->m_uart->errorOutPut[1] = this->m_ipc->trackposy;
			}
}


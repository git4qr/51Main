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
	printf("CMsgProcess====>Create!\n");
	  // IPC communication
	 m_ipc = new CIPCProc();
      m_ipc->Create();
      //Jos Create
   	m_jos = new CJosStick();
   	m_jos->Create();
  	//uart Create
  	m_uart = new CUartProcess();
  	m_uart->Create();

	int iRet = 0;
	MSGDRIV_create();
	iRet = Work_queCreate();
	if (iRet	){
		fprintf(stdout,"Create que error!!\r\n");
	}
	configAvtFromFile();
	PlatformCtrl_CreateParams_Init(&m_pltParams, &m_cfgPlatParam);
	printf("PlatCtrl is OK !!!\n");

	//OSA_semCreate(m_semHndl, 1, 0);
	PlantTrackerInputPara();
	OSA_assert(m_plt == NULL);
	m_plt = PlatformCtrl_Create(&m_pltParams);
	OSA_assert(m_plt != NULL);
	m_ptz = new CPTZControl();
    //m_ptz->Create();


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
#if 0
int CMsgProcess::Config_threadCreate()
{
	int iRet;
	iRet = OSA_thrCreate(&m_thrRead, readCfgParamFunc , 0, 0, (void*)this);
		if(iRet != 0)
	fprintf(stdout,"config creat failed\n");
		return 0;
}
#endif
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
						case Cmd_IPC_Config:
							MSGDRIV_send(MSGID_IPC_Config, 0);
							printf("CMsgProcess====>recv config\n");
								gettimeofday(&ptz111, NULL);
								printf("recv config++++%d++++ %d \n", ptz111.tv_sec,ptz111.tv_usec);
						break;
						case Cmd_IPC_OSD:
							MSGDRIV_send(MSGID_IPC_OSD, 0);
							break;
						case Cmd_IPC_UTC:
							MSGDRIV_send(MSGID_IPC_UTC, 0);
							break;
						case Cmd_IPC_Camera:
							MSGDRIV_send(MSGID_IPC_Camera, 0);
							break;
						case Cmd_Mesg_config_Write:
							MSGDRIV_send(MSGID_EXT_INPUT_configWrite, 0);
							break;
						default:
							fprintf(stdout,"INFO: can not excute here\r\n");
					}
}

int  CMsgProcess::configAvtFromFile()
{
	m_ipc->ipc_OSD = m_ipc->getOSDSharedMem();
	m_ipc->ipc_UTC = m_ipc->getUTCSharedMem();

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
				for(int i=0; i<configId_Max; i++){
									sprintf(cfg_avt, "cfg_avt_%d", i);
									cfg_value[i] = (int)fr[cfg_avt];
									//printf(" update cfg [%d] %d \n", i, cfg_value[i]);
								}
				{ //uart
				typedef struct{
					string UartDev;
					int BaudRate;
					char DataBits;
					char ParityBits;
					string StopBits;
					int FlowCtrl;
				}uratParam;

				} // uart



				{   // plat

				m_cfgPlatParam.scalarX = (float)fr["cfg_avt_2"];

				m_cfgPlatParam.scalarY = (float)fr["cfg_avt_3"];

				m_cfgPlatParam.demandMaxX = (float)fr["cfg_avt_4"];

				m_cfgPlatParam.demandMinX = (float)fr["cfg_avt_5"];

				m_cfgPlatParam.demandMaxY = (float)fr["cfg_avt_6"];

				m_cfgPlatParam.demandMinY = (float)fr["cfg_avt_7"];

				m_cfgPlatParam.bleedUsed = (int)fr["cfg_avt_12"];
				printf("CMsgProcess===> bleedUsed = %d\n", m_cfgPlatParam.bleedUsed);

				m_cfgPlatParam.bleedX = (float)fr["cfg_avt_13"];
				printf("CMsgProcess===> bleedX = %f\n", m_cfgPlatParam.bleedX);

				m_cfgPlatParam.bleedY = (float)fr["cfg_avt_14"];
				printf("CMsgProcess===> bleedY = %f\n", m_cfgPlatParam.bleedY);
				} //   plat




				{//pid

					m_cfgPlatParam.m__cfg_platformFilterParam.P0 = (float)fr["cfg_avt_17"];

					m_cfgPlatParam.m__cfg_platformFilterParam.P1 = (float)fr["cfg_avt_18"];

					m_cfgPlatParam.m__cfg_platformFilterParam.P2 = (float)fr["cfg_avt_19"];

					m_cfgPlatParam.m__cfg_platformFilterParam.L1 = (float)fr["cfg_avt_20"];

					m_cfgPlatParam.m__cfg_platformFilterParam.L2 = (float)fr["cfg_avt_21"];

					m_cfgPlatParam.m__cfg_platformFilterParam.G = (float)fr["cfg_avt_22"];
					printf("CMsgProcess===> G = %f\n", m_cfgPlatParam.m__cfg_platformFilterParam.G);

					m_cfgPlatParam.m__cfg_platformFilterParam.P02 = (float)fr["cfg_avt_23"];

					m_cfgPlatParam.m__cfg_platformFilterParam.P12 = (float)fr["cfg_avt_24"];

					m_cfgPlatParam.m__cfg_platformFilterParam.P22 = (float)fr["cfg_avt_25"];

					m_cfgPlatParam.m__cfg_platformFilterParam.L12 = (float)fr["cfg_avt_26"];

					m_cfgPlatParam.m__cfg_platformFilterParam.L22 = (float)fr["cfg_avt_27"];

					m_cfgPlatParam.m__cfg_platformFilterParam.G2 = (float)fr["cfg_avt_28"];

				}//pid




				{ // osd

				m_ipc->ipc_OSD->MAIN_Sensor = (int)fr["cfg_avt_192"];

				m_ipc->ipc_OSD->Timedisp_9 = (int)fr["cfg_avt_193"];

				m_ipc->ipc_OSD->OSD_text_show = (int)fr["cfg_avt_194"];

				m_ipc->ipc_OSD->OSD_text_color = (int)fr["cfg_avt_195"];
				printf("OSD_text_color %x\n", m_ipc->ipc_OSD->OSD_text_color);

				m_ipc->ipc_OSD->OSD_text_alpha = (int)fr["cfg_avt_196"];

				m_ipc->ipc_OSD->OSD_text_font = (int)fr["cfg_avt_197"];

				m_ipc->ipc_OSD->OSD_text_size = (int)fr["cfg_avt_198"];

				m_ipc->ipc_OSD->OSD_draw_show = (int)fr["cfg_avt_199"];

				m_ipc->ipc_OSD->OSD_draw_color = (int)fr["cfg_avt_200"];

				m_ipc->ipc_OSD->CROSS_AXIS_WIDTH = (int)fr["cfg_avt_201"];

				m_ipc->ipc_OSD->CROSS_AXIS_HEIGHT = (int)fr["cfg_avt_202"];

				m_ipc->ipc_OSD->Picp_CROSS_AXIS_WIDTH = (int)fr["cfg_avt_203"];

				m_ipc->ipc_OSD->Picp_CROSS_AXIS_HEIGHT = (int)fr["cfg_avt_204"];

				m_ipc->ipc_OSD->ch0_acqRect_width = (int)fr["cfg_avt_205"];

				m_ipc->ipc_OSD->ch1_acqRect_width = (int)fr["cfg_avt_206"];

				m_ipc->ipc_OSD->ch2_acqRect_width = (int)fr["cfg_avt_207"];

				m_ipc->ipc_OSD->ch3_acqRect_width = (int)fr["cfg_avt_208"];

				m_ipc->ipc_OSD->ch4_acqRect_width = (int)fr["cfg_avt_209"];

				m_ipc->ipc_OSD->ch5_acqRect_width = (int)fr["cfg_avt_210"];

				m_ipc->ipc_OSD->ch0_acqRect_height = (int)fr["cfg_avt_211"];

				m_ipc->ipc_OSD->ch1_acqRect_height = (int)fr["cfg_avt_212"];

				m_ipc->ipc_OSD->ch2_acqRect_height = (int)fr["cfg_avt_213"];

				m_ipc->ipc_OSD->ch3_acqRect_height = (int)fr["cfg_avt_214"];

				m_ipc->ipc_OSD->ch4_acqRect_height = (int)fr["cfg_avt_215"];

				m_ipc->ipc_OSD->ch5_acqRect_height = (int)fr["cfg_avt_216"];

				m_ipc->ipc_OSD->ch0_aim_width = (int)fr["cfg_avt_217"];

				m_ipc->ipc_OSD->ch1_aim_width = (int)fr["cfg_avt_218"];

				m_ipc->ipc_OSD->ch2_aim_width = (int)fr["cfg_avt_219"];

				m_ipc->ipc_OSD->ch3_aim_width = (int)fr["cfg_avt_220"];

				m_ipc->ipc_OSD->ch4_aim_width = (int)fr["cfg_avt_221"];

				m_ipc->ipc_OSD->ch5_aim_width = (int)fr["cfg_avt_222"];

				m_ipc->ipc_OSD->ch6_aim_width = (int)fr["cfg_avt_223"];

				m_ipc->ipc_OSD->ch1_aim_height = (int)fr["cfg_avt_224"];

				m_ipc->ipc_OSD->ch2_aim_height = (int)fr["cfg_avt_225"];

				m_ipc->ipc_OSD->ch3_aim_height = (int)fr["cfg_avt_226"];

				m_ipc->ipc_OSD->ch4_aim_height = (int)fr["cfg_avt_227"];

				m_ipc->ipc_OSD->ch5_aim_height = (int)fr["cfg_avt_228"];
				printf("ch5_aim_height %d\n", m_ipc->ipc_OSD->ch5_aim_height);
				}//osd

				{ //UTC_TRK
					m_ipc->ipc_UTC->occlusion_thred = (float)fr["cfg_avt_128"];//9--0
					m_ipc->ipc_UTC->retry_acq_thred = (float)fr["cfg_avt_129"];
					m_ipc->ipc_UTC->up_factor = (float)fr["cfg_avt_130"];
					m_ipc->ipc_UTC->res_distance = (float)fr["cfg_avt_131"];
					m_ipc->ipc_UTC->res_area = (float)fr["cfg_avt_132"];
					m_ipc->ipc_UTC->gapframe = (float)fr["cfg_avt_133"];
					m_ipc->ipc_UTC->enhEnable = (float)fr["cfg_avt_134"];
					m_ipc->ipc_UTC->cliplimit = (float)fr["cfg_avt_135"];
					m_ipc->ipc_UTC->dictEnable = (float)fr["cfg_avt_136"];
					m_ipc->ipc_UTC->moveX = (float)fr["cfg_avt_137"];
					m_ipc->ipc_UTC->moveY = (float)fr["cfg_avt_138"];
					m_ipc->ipc_UTC->moveX2 = (float)fr["cfg_avt_139"];
					m_ipc->ipc_UTC->moveY2 = (float)fr["cfg_avt_140"];
					m_ipc->ipc_UTC->segPixelX = (float)fr["cfg_avt_141"];
					m_ipc->ipc_UTC->segPixelY = (float)fr["cfg_avt_142"];
					m_ipc->ipc_UTC->algOsdRect_Enable = (float)fr["cfg_avt_143"];  //9--15

					m_ipc->ipc_UTC->ScalerLarge = (float)fr["cfg_avt_144"];//10--0
					m_ipc->ipc_UTC->ScalerMid = (float)fr["cfg_avt_145"];
					m_ipc->ipc_UTC->ScalerSmall = (float)fr["cfg_avt_146"];
					m_ipc->ipc_UTC->Scatter = (float)fr["cfg_avt_147"];
					m_ipc->ipc_UTC->ratio = (float)fr["cfg_avt_148"];
					m_ipc->ipc_UTC->FilterEnable = (float)fr["cfg_avt_149"];
					m_ipc->ipc_UTC->BigSecEnable = (float)fr["cfg_avt_150"];
					m_ipc->ipc_UTC->SalientThred = (float)fr["cfg_avt_151"];
					m_ipc->ipc_UTC->ScalerEnable = (float)fr["cfg_avt_152"];
					m_ipc->ipc_UTC->DynamicRatioEnable = (float)fr["cfg_avt_153"];
					m_ipc->ipc_UTC->acqSize_width = (float)fr["cfg_avt_154"];
					m_ipc->ipc_UTC->acqSize_height = (float)fr["cfg_avt_155"];
					m_ipc->ipc_UTC->TrkAim43_Enable = (float)fr["cfg_avt_156"];
					m_ipc->ipc_UTC->SceneMVEnable = (float)fr["cfg_avt_157"];
					m_ipc->ipc_UTC->BackTrackEnable = (float)fr["cfg_avt_158"];
					m_ipc->ipc_UTC->bAveTrkPos = (float)fr["cfg_avt_159"]; //10--15

					m_ipc->ipc_UTC->fTau = (float)fr["cfg_avt_160"]; //11--0
					m_ipc->ipc_UTC->buildFrms = (float)fr["cfg_avt_161"];
					m_ipc->ipc_UTC->LostFrmThred = (float)fr["cfg_avt_162"];
					m_ipc->ipc_UTC->histMvThred = (float)fr["cfg_avt_163"];
					m_ipc->ipc_UTC->detectFrms = (float)fr["cfg_avt_164"];
					m_ipc->ipc_UTC->stillFrms = (float)fr["cfg_avt_165"];
					m_ipc->ipc_UTC->stillThred = (float)fr["cfg_avt_166"];
					m_ipc->ipc_UTC->bKalmanFilter = (float)fr["cfg_avt_167"];
					m_ipc->ipc_UTC->xMVThred = (float)fr["cfg_avt_168"];
					m_ipc->ipc_UTC->yMVThred = (float)fr["cfg_avt_169"];
					m_ipc->ipc_UTC->xStillThred = (float)fr["cfg_avt_170"];
					m_ipc->ipc_UTC->yStillThred = (float)fr["cfg_avt_171"];
					m_ipc->ipc_UTC->slopeThred = (float)fr["cfg_avt_172"];
					m_ipc->ipc_UTC->kalmanHistThred = (float)fr["cfg_avt_173"];
					m_ipc->ipc_UTC->kalmanCoefQ = (float)fr["cfg_avt_174"];
					m_ipc->ipc_UTC->kalmanCoefR = (float)fr["cfg_avt_175"]; //11--15

					m_ipc->ipc_UTC->Enhmod_0 = (float)fr["cfg_avt_176"]; //12--0
					m_ipc->ipc_UTC->Enhparm_1 = (float)fr["cfg_avt_177"];
					m_ipc->ipc_UTC->Mmtdparm_2 = (float)fr["cfg_avt_178"];
					m_ipc->ipc_UTC->Mmtdparm_3 = (float)fr["cfg_avt_179"];
					m_ipc->ipc_UTC->Mmtdparm_4 = (float)fr["cfg_avt_180"];
					m_ipc->ipc_UTC->Mmtdparm_5 = (float)fr["cfg_avt_181"];
					m_ipc->ipc_UTC->Mmtdparm_6 = (float)fr["cfg_avt_182"];
					m_ipc->ipc_UTC->Mmtdparm_7 = (float)fr["cfg_avt_183"];
					m_ipc->ipc_UTC->Mmtdparm_8 = (float)fr["cfg_avt_184"]; //12--8
					printf("CMsgProcess=====>ipc_UTC->Mmtdparm_8 = %d\n", m_ipc->ipc_UTC->Mmtdparm_8 );
				} //UTC_TRK
							}else
								return -1;
			}
		}

	string cfgCameraFile;
		int cfgId_Max = 671;
		char  cfg_camera[20] = "cfg_avt_";
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
										cfg_value[j] << fr[cfg_camera];
										//printf(" update cfg [%d] %d \n", j, cfg_value[j]);
									}
								}else
									return -1;
				}
			}
		printf("send msg start!\n");
		m_uart->ReadProfile();
		printf("send msg is OK !!!\n");
	}

void CMsgProcess::modifierAVTProfile(int block, int field, int value)
{
	m_ipc->ipc_OSD = m_ipc->getOSDSharedMem();
	m_ipc->ipc_UTC = m_ipc->getUTCSharedMem();

	int check = ((block -1) * 16 + field);
	//cfg_value[check] = value;
	switch(check)
	{
	case 2:
		m_cfgPlatParam.scalarX = value;
		break;
	case 3:
		m_cfgPlatParam.scalarY = value;
		break;
	case 4:
		m_cfgPlatParam.demandMaxX = value;
		break;
	case 5:
		m_cfgPlatParam.demandMinX = value;
		break;
	case 6:
		m_cfgPlatParam.demandMaxY = value;
		break;
	case 7:
		m_cfgPlatParam.demandMinY = value;
		break;
	case 12:
		m_cfgPlatParam.bleedUsed = value;
		break;
	case 13:
		m_cfgPlatParam.bleedX = value;
		break;
	case 14:
		m_cfgPlatParam.bleedY = value;
		break;
	case 17:
		m_cfgPlatParam.m__cfg_platformFilterParam.P0 = value;
		break;
	case 18:
		m_cfgPlatParam.m__cfg_platformFilterParam.P1 = value;
		break;
	case 19:
		m_cfgPlatParam.m__cfg_platformFilterParam.P2 = value;
		break;
	case 20:
		m_cfgPlatParam.m__cfg_platformFilterParam.L1 = value;
		break;
	case 21:
		m_cfgPlatParam.m__cfg_platformFilterParam.L2 = value;
		break;
	case 22:
			m_cfgPlatParam.m__cfg_platformFilterParam.G = value;
		break;
	case 23:
		m_cfgPlatParam.m__cfg_platformFilterParam.P02 = value;
		break;
	case 24:
		m_cfgPlatParam.m__cfg_platformFilterParam.P12 = value;
		break;
	case 25:
		m_cfgPlatParam.m__cfg_platformFilterParam.P22 = value;
		break;
	case 26:
		m_cfgPlatParam.m__cfg_platformFilterParam.L12 = value;
		break;
	case 27:
		m_cfgPlatParam.m__cfg_platformFilterParam.L22 = value;
		break;
	case 28:
			m_cfgPlatParam.m__cfg_platformFilterParam.G2 = value;
		break;
	case 128:
		m_ipc->ipc_UTC->occlusion_thred = value;
		break;
	case 129:
		m_ipc->ipc_UTC->retry_acq_thred = value;
		break;
	case 130:
		m_ipc->ipc_UTC->up_factor = value;
		break;
	case 131:
		m_ipc->ipc_UTC->res_distance = value;
		break;
	case 132:
		m_ipc->ipc_UTC->res_area = value;
		break;
	case 133:
		m_ipc->ipc_UTC->gapframe = value;
		break;
	case 134:
		m_ipc->ipc_UTC->enhEnable = value;
		break;
	case 135:
		m_ipc->ipc_UTC->cliplimit = value;
		break;
	case 136:
		m_ipc->ipc_UTC->dictEnable = value;
		break;
	case 137:
		m_ipc->ipc_UTC->moveX = value;
		break;
	case 138:
		m_ipc->ipc_UTC->moveY = value;
		break;
	case 139:
		m_ipc->ipc_UTC->moveX2 = value;
		break;
	case 140:
		m_ipc->ipc_UTC->moveY2 = value;
		break;
	case 141:
		m_ipc->ipc_UTC->segPixelX = value;
		break;
	case 142:
		m_ipc->ipc_UTC->segPixelY = value;
		break;
	case 143:
		m_ipc->ipc_UTC->algOsdRect_Enable = value;
		break;
	case 144:
		m_ipc->ipc_UTC->ScalerLarge = value;
		break;
	case 145:
		m_ipc->ipc_UTC->ScalerMid = value;
		break;
	case 146:
		m_ipc->ipc_UTC->ScalerSmall = value;
		break;
	case 147:
		m_ipc->ipc_UTC->Scatter = value;
		break;
	case 148:
		m_ipc->ipc_UTC->ratio = value;
		break;
	case 149:
		m_ipc->ipc_UTC->FilterEnable = value;
		break;
	case 150:
		m_ipc->ipc_UTC->BigSecEnable = value;
		break;
	case 151:
		m_ipc->ipc_UTC->SalientThred = value;
		break;
	case 152:
		m_ipc->ipc_UTC->ScalerEnable = value;
		break;
	case 153:
		m_ipc->ipc_UTC->DynamicRatioEnable = value;
		break;
	case 154:
		m_ipc->ipc_UTC->acqSize_width = value;
		break;
	case 155:
		m_ipc->ipc_UTC->acqSize_height = value;
		break;
	case 156:
		m_ipc->ipc_UTC->TrkAim43_Enable = value;
		break;
	case 157:
		m_ipc->ipc_UTC->SceneMVEnable = value;
		break;
	case 158:
		m_ipc->ipc_UTC->BackTrackEnable = value;
		break;
	case 159:
		m_ipc->ipc_UTC->bAveTrkPos = value;
		break;
	case 160:
		m_ipc->ipc_UTC->fTau = value;
		break;
	case 161:
		m_ipc->ipc_UTC->buildFrms = value;
		break;
	case 162:
		m_ipc->ipc_UTC->LostFrmThred = value;
		break;
	case 163:
		m_ipc->ipc_UTC->histMvThred = value;
		break;
	case 164:
		m_ipc->ipc_UTC->detectFrms = value;
		break;
	case 165:
		m_ipc->ipc_UTC->stillFrms = value;
		break;
	case 166:
		m_ipc->ipc_UTC->stillThred = value;
		break;
	case 167:
		m_ipc->ipc_UTC->bKalmanFilter = value;
		break;
	case 168:
		m_ipc->ipc_UTC->xMVThred = value;
		break;
	case 169:
		m_ipc->ipc_UTC->yMVThred = value;
		break;
	case 170:
		m_ipc->ipc_UTC->xStillThred = value;
		break;
	case 171:
		m_ipc->ipc_UTC->yStillThred = value;
		break;
	case 172:
		m_ipc->ipc_UTC->slopeThred = value;
		break;
	case 173:
		m_ipc->ipc_UTC->kalmanHistThred = value;
		break;
	case 174:
		m_ipc->ipc_UTC->kalmanCoefQ = value;
		break;
	case 175:
		m_ipc->ipc_UTC->kalmanCoefR = value;
		break;
	case 176:
		m_ipc->ipc_UTC->Enhmod_0 = value;
		break;
	case 177:
		m_ipc->ipc_UTC->Enhparm_1 = value;
		break;
	case 178:
		m_ipc->ipc_UTC->Mmtdparm_2 = value;
		break;
	case 179:
		m_ipc->ipc_UTC->Mmtdparm_3 = value;
		break;
	case 180:
		m_ipc->ipc_UTC->Mmtdparm_4 = value;
		break;
	case 181:
		m_ipc->ipc_UTC->Mmtdparm_5 = value;
		break;
	case 182:
		m_ipc->ipc_UTC->Mmtdparm_6 = value;
		break;
	case 183:
		m_ipc->ipc_UTC->Mmtdparm_7 = value;
		break;
	case 184:
		m_ipc->ipc_UTC->Mmtdparm_8 = value;
		break;

	case 192:
		m_ipc->ipc_OSD->MAIN_Sensor = value;
		break;
	case 193:
		m_ipc->ipc_OSD->Timedisp_9 = value;
		break;
	case 194:
		m_ipc->ipc_OSD->OSD_text_show = value;
		break;
	case 195:
		m_ipc->ipc_OSD->OSD_text_color = value;
		break;
	case 196:
		m_ipc->ipc_OSD->OSD_text_alpha = value;
		break;
	case 197:
		m_ipc->ipc_OSD->OSD_text_font = value;
		break;
	case 198:
		m_ipc->ipc_OSD->OSD_text_size = value;
		break;
	case 199:
		m_ipc->ipc_OSD->OSD_draw_show = value;
		break;
	case 200:
		m_ipc->ipc_OSD->OSD_draw_color = value;
		break;
	case 201:
		m_ipc->ipc_OSD->CROSS_AXIS_WIDTH = value;
		break;
	case 202:
		m_ipc->ipc_OSD->CROSS_AXIS_HEIGHT = value;
		break;
	case 203:
		m_ipc->ipc_OSD->Picp_CROSS_AXIS_WIDTH = value;
		break;
	case 204:
		m_ipc->ipc_OSD->Picp_CROSS_AXIS_HEIGHT = value;
		break;
	case 205:
		m_ipc->ipc_OSD->ch0_acqRect_width =value;
		break;
	case 206:
		m_ipc->ipc_OSD->ch1_acqRect_width = value;
		break;
	case 207:
		m_ipc->ipc_OSD->ch2_acqRect_width = value;
		break;
	case 208:
		m_ipc->ipc_OSD->ch3_acqRect_width = value;
		break;
	case 209:
		m_ipc->ipc_OSD->ch4_acqRect_width = value;
		break;
	case 210:
		m_ipc->ipc_OSD->ch5_acqRect_width = value;
		break;
	case 211:
		m_ipc->ipc_OSD->ch0_acqRect_height = value;
		break;
	case 212:
		m_ipc->ipc_OSD->ch1_acqRect_height = value;
		break;
	case 213:
		m_ipc->ipc_OSD->ch2_acqRect_height = value;
		break;
	case 214:
		m_ipc->ipc_OSD->ch3_acqRect_height = value;
		break;
	case 215:
		m_ipc->ipc_OSD->ch4_acqRect_height = value;
		break;
	case 216:
		m_ipc->ipc_OSD->ch5_acqRect_height  = value;
		break;
	case 217:
		m_ipc->ipc_OSD->ch0_aim_width = value;
		break;
	case 218:
		m_ipc->ipc_OSD->ch1_aim_width = value;
		break;
	case 219:
		m_ipc->ipc_OSD->ch2_aim_width = value;
		break;
	case 220:
		m_ipc->ipc_OSD->ch3_aim_width = value;
		break;
	case 221:
		m_ipc->ipc_OSD->ch4_aim_width = value;
		break;
	case 222:
		m_ipc->ipc_OSD->ch5_aim_width = value;
		break;
	case 223:
		m_ipc->ipc_OSD->ch6_aim_width = value;
		break;
	case 224:
		m_ipc->ipc_OSD->ch1_aim_height = value;
		break;
	case 225:
		m_ipc->ipc_OSD->ch2_aim_height = value;
		break;
	case 226:
		m_ipc->ipc_OSD->ch3_aim_height = value;
		break;
	case 227:
		m_ipc->ipc_OSD->ch4_aim_height = value;
		break;
	case 228:
		m_ipc->ipc_OSD->ch5_aim_height = value;
		break;
	default:
		break;
	}
	if( check >= 192 && check <= 228)
		m_uart->ReadShmOSD();
	else if(check >= 128 && check <= 184)
		m_uart->ReadShmUTC();
}

int CMsgProcess::updataProfile()
{
	string cfgAvtFile;
	int configId_Max = 256;
	char  cfg_avt[20] = "cfg_avt_";
	cfgAvtFile = "Profile.yml";
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt+");

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
									fr[cfg_avt] << cfg_value[i];
				}

#if 0
									{   // plat
									fr["cfg_avt_2"] << m_cfgPlatParam.scalarX;

									fr["cfg_avt_3"] << m_cfgPlatParam.scalarY;

									fr["cfg_avt_4"] << m_cfgPlatParam.demandMaxX;

									fr["cfg_avt_5"] <<m_cfgPlatParam.demandMinX;

									fr["cfg_avt_6"] << m_cfgPlatParam.demandMaxY;

									fr["cfg_avt_7"] << m_cfgPlatParam.demandMinY;

									fr["cfg_avt_12"] << m_cfgPlatParam.bleedUsed;
									printf("updataProfile===> bleedUsed = %d\n", m_cfgPlatParam.bleedUsed);

									fr["cfg_avt_13"] << m_cfgPlatParam.bleedX;
									printf("updataProfile===> bleedX = %f\n", m_cfgPlatParam.bleedX);

									fr["cfg_avt_14"] << m_cfgPlatParam.bleedY;
									printf("updataProfile===> bleedY = %f\n", m_cfgPlatParam.bleedY);
									} //   plat




									{//pid

										fr["cfg_avt_17"] << m_cfgPlatParam.m__cfg_platformFilterParam.P0;

										fr["cfg_avt_18"] << m_cfgPlatParam.m__cfg_platformFilterParam.P;

										fr["cfg_avt_19"] << m_cfgPlatParam.m__cfg_platformFilterParam.P2;

										fr["cfg_avt_20"] << m_cfgPlatParam.m__cfg_platformFilterParam.L1;

										fr["cfg_avt_21"] << m_cfgPlatParam.m__cfg_platformFilterParam.L2;

										fr["cfg_avt_22"] << m_cfgPlatParam.m__cfg_platformFilterParam.G;
										printf("updataProfile===> G = %f\n", m_cfgPlatParam.m__cfg_platformFilterParam.G);

										fr["cfg_avt_23"] << m_cfgPlatParam.m__cfg_platformFilterParam.P02;

										fr["cfg_avt_24"] << m_cfgPlatParam.m__cfg_platformFilterParam.P12;

										fr["cfg_avt_25"] << m_cfgPlatParam.m__cfg_platformFilterParam.P22;

										fr["cfg_avt_26"] << m_cfgPlatParam.m__cfg_platformFilterParam.L12;

										fr["cfg_avt_27"] << m_cfgPlatParam.m__cfg_platformFilterParam.L22;

										fr["cfg_avt_28"] << m_cfgPlatParam.m__cfg_platformFilterParam.G2;
									}//pid
#endif
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
				FileStorage fr(cfgCameraFile, FileStorage::WRITE);
				if(fr.isOpened()){
					for(int j=256; j<cfgId_Max;j++){
										sprintf(cfg_camera, "cfg_avt_%d", j);
										fr[cfg_camera] << cfg_value[j];
										printf(" update cfg [%d] %d \n", j, cfg_value[j]);
									}
								}else
									return -1;
				}
			}
#endif
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

void CMsgProcess::Change_avtStatus()
{
	if(m_SetRealTime == 0)
				m_SetRealTime = 1;
				else
					m_SetRealTime = 0;
				realtime_avtStatus();
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


#ifndef _PLATFORM_CONTROL_H
#define _PLATFORM_CONTROL_H

#include "virtualDevice.h"
#include "deviceUser.h"
#include "platformFilter.h"
#include "sensorComp.h"

#define VIRTUAL_DEVICE_COUNT		(24)
#define ANALOG_DEVICE_COUNT			(16)
#define SENSOR_COUNT				(2)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __INLINE
#    define __INLINE static __inline 
#endif

#ifndef ONLYREAD
#	define ONLYREAD
#endif
#define PI 3.14159269798

typedef enum{
	DevUsr_AcqWinPosXInput =0,
	DevUsr_AcqWinPosYInput,
	DevUsr_AcqWinSizeXInput,
	DevUsr_AcqWinSizeYInput,
	DevUsr_TrkWinSizeXInput,
	DevUsr_TrkWinSizeYInput,
	DevUsr_AcqPltRateXInput,
	DevUsr_AcqPltRateYInput,
	DevUsr_AcqJoystickXInput,
	DevUsr_AcqJoystickYInput,
	DevUsr_AimpointOffsetXInput,
	DevUsr_AimpointOffsetYInput,
	DevUsr_AcqPositionDemandXInput,
	DevUsr_AcqPositionDemandYInput,
	DevUsr_Sensor1ZoomFeedbackInput,
	DevUsr_AimpointRefineXInput,
	DevUsr_AimpointRefineYInput,
	DevUsr_AcqPreprocessorManualOffsetInput,
	DevUsr_PltCoastXInput,
	DevUsr_PltCoastYInput,
	DevUsr_TrkPreprocessor,
	DevUsr_MAX
}eDevUsr;

typedef enum{
	//ƽ̨���Ϊ0
	AcqOutputType_Zero = 0,
	
	//ƽ̨���Ϊң��ԭʼ����
	AcqOutputType_JoystickInput,
	
	// ƽ̨���Ϊң��������״�������滯
	AcqOutputType_ShapedAndGained,
	
	//ƽ̨�������Ϊң���������״�����滯�ͻ��
	AcqOutputType_ShapedAndGainedAndIntegrated,
	
	//demand in Block 88, Fields 11 and 12. 
	//ƽ̨����ɿ�88��11��12��λ���������
	AcqOutputType_DeterminedByPosition,					
	
	//but track filters are initialized by incoming platform data (if available). 
	//ƽ̨�����Ϊ0�����Ǹ����˲����������ƽ̨��ݣ����ɻ�ã�����ʼ��
	AcqOutputType_ZeroInitFilter,
	
	//(used when platform is under external control in acquisition).
	//ƽ̨����������ƽ̨�����ȷ������ƽ̨�ڲ���ģʽ�´����ⲿ����ʱʹ�õ���ݣ�
	AcqOutputType_DeterminedByIncomingPlatformData,     
	
	AcqOutputType_Max
}eAcqOutputType;

typedef enum
{
	//0 Disabled ��ֹ
	PlatStateType_Disable = 0,

	//1 Acquire ����
	PlatStateType_Acquire =1 ,

	//2 Lock ��
	PlatStateType_Trk_Lock = 2,

	//3 Breaklock Stage 1 �ж���׶�1
	PlatStateType_Trk_Break_Lock_1 = 3,

	//4 Breaklock Stage 2 �ж���׶�2
	PlatStateType_Trk_Break_Lock_2 = 4,

	//5 Force Coast ǿ�ƹ��Ը���*/
	PlatStateType_Trk_Force_Coast  = 5

}eTrkStateType,ePlatStateType;

typedef enum{
	Bleed_Disable        = 0,
	Bleed_BrosightError  = 1,
	Bleed_PlatformDemand = 2
}ePlatfromCtrlBleed;

/* ����ƽ̨���ƽӿ� */
typedef struct{
	int iSensor;

	/*Track Algorithm State �����㷨״̬��ָʾ��ǰ�ڲ������㷨��״̬
		0 Disabled ��ֹ
		1 Acquire ����
		2 Lock ��
		3 Breaklock Stage 1 �ж���׶�1
		4 Breaklock Stage 2 �ж���׶�2
		5 Force Coast ǿ�ƹ��Ը���*/
	ONLYREAD int iTrkAlgState;

	/*Boresight Position X ����λ��X*/
	ONLYREAD float fBoresightPositionX;

	/*Boresight Position Y ����λ��Y*/
	ONLYREAD float fBoresightPositionY;
	
	/* Target to Boresight Error X Ŀ���������X */
	ONLYREAD float fTargetBoresightErrorX;
	
	/* Target to Boresight Error Y Ŀ���������Y */
	ONLYREAD float fTargetBoresightErrorY;
	
	/*Window Position X ����λ��X*/
	ONLYREAD float fWindowPositionX;
	
	/*Window Position Y ����λ��Y*/
	ONLYREAD float fWindowPositionY;
	
	/*Window Size X���ڳߴ�X*/
	ONLYREAD float fWindowSizeX;
	
	/*Window Size Y���ڳߴ�Y*/
	ONLYREAD float fWindowSizeY;
	
	/*Platform Demand X ƽ̨����X*/
	ONLYREAD float fPlatformDemandX;
	
	/*Platform Demand Y ƽ̨����Y*/
	ONLYREAD float fPlatformDemandY;
}PLATFORMCTRL_Output;

typedef struct{
	/*Track Algorithm State �����㷨״̬��ָʾ��ǰ�ڲ������㷨��״̬
		0 Disabled ��ֹ
		1 Acquire ����
		2 Lock ��
		3 Breaklock Stage 1 �ж���׶�1
		4 Breaklock Stage 2 �ж���׶�2
		5 Force Coast ǿ�ƹ��Ը���*/
	ONLYREAD int iTrkAlgState;
	
	/*Boresight Position X ����λ��X*/
	ONLYREAD float fBoresightPositionX;
	
	/*Boresight Position Y ����λ��Y*/
	ONLYREAD float fBoresightPositionY;
	
	/* Target to Bore sight Error X Ŀ���������X */
	ONLYREAD float fTargetBoresightErrorX;
	
	/* Target to Bore sight Error Y Ŀ���������Y */
	ONLYREAD float fTargetBoresightErrorY;
	
	/*Acquire Window Position X ����λ��X*/
	ONLYREAD float fAcqWindowPositionX;
	
	/*Acquire Window Position Y ����λ��Y*/
	ONLYREAD float fAcqWindowPositionY;
	
	/*Acquire Window Size X���ڳߴ�X*/
	ONLYREAD float fAcqWindowSizeX;
	
	/*Acquire Window Size Y���ڳߴ�Y*/
	ONLYREAD float fAcqWindowSizeY;
	
	/*Target Size X Ŀ��ߴ�X*/
	ONLYREAD float fTargetSizeX;
	
	/*Target Size Y Ŀ��ߴ�Y*/
	ONLYREAD float fTargetSizeY;

}PLATFORMCTRL_TrackerInput;

typedef  struct {
	void *object;
	ONLYREAD float devUsrInput[DevUsr_MAX];
	ONLYREAD float localInput[DevUsr_MAX];
	ONLYREAD float virtalInput[VIRTUAL_DEVICE_COUNT];
	ONLYREAD float analogInput[ANALOG_DEVICE_COUNT];
	ONLYREAD PLATFORMCTRL_TrackerInput trackerInput;
	ONLYREAD PLATFORMCTRL_Output output;
}PLATFORMCTRL_Interface;

typedef PLATFORMCTRL_Interface * HPLTCTRL;

/* ������Ƶ���������� */
typedef SensorComp_CreateParams SensorParams;
/* ����ģ���豸���� */
typedef AnalogDevice_CreateParams AnalogDeviceParam;
/* ���������豸���� */
typedef VirtualDevice_CreateParams VirtualDeviceParam;
/* �����豸�û����� */
typedef DeviceUser_CreateParams DeviceUserParam;
/* ����ƽ̨�˲������� */
typedef PlatformFilter_CreateParams PlatformFilterParam;
typedef PlatformFilter_InitParams 	m_PlatformFilterParam;
/* �����ֱ��ٶ�������� */
typedef struct {
	float fOffset_X;
	float fOffset_Y;
	float fDeadband;
	float fCutpoint;
	float fInputGain_X;
	float fInputGain_Y;
	float fPlatformAcquisitionModeGain_X;
	float fPlatformAcquisitionModeGain_Y;

	//0: Use Cut point ʹ��ת�۵�; 1: Use Look Up Table ʹ�ò��
	int   iShapingMode;
	float fPlatformIntegratingGain_X;
	float fPlatformIntegratingGain_Y;
	float fPlatformIntegratingDecayGain;
}JoystickRateDemandParam;

typedef struct
{
	/* ��Ƶ���������� */
	SensorParams sensorParams[SENSOR_COUNT];
	/* �����豸���� */
	AnalogDeviceParam	analogDeviceParams[ANALOG_DEVICE_COUNT];
	/* �����豸���� */
	VirtualDeviceParam	virtualDeviceParams[VIRTUAL_DEVICE_COUNT];
	/* �豸�û����� */
	DeviceUserParam deviceUsrParam[DevUsr_MAX];
	/* �˲������� */
	PlatformFilterParam platformFilterParam[2][2];
	m_PlatformFilterParam m__cfg_platformFilterParam;
	/* �ֱ��ٶ�������� */
	JoystickRateDemandParam joystickRateDemandParam;

	/* ƽ̨��������ϵ�� */
	float scalarX;
	float scalarY;
	float demandMaxX;
	float demandMinX;
	float demandMaxY;
	float demandMinY;
	float deadbandX;
	float deadbandY;
	float driftX;
	float driftY;

	/* ƽ̨Bleedģʽ */
	int bleedUsed;
	float bleedX;
	float bleedY;

	/* ��ǰ��Ƶ������ */
	int iSensorInit;

	/* ��ǰ�ӳ� */
	float fFov[SENSOR_COUNT];

	/* ����ģʽƽ̨������� */
	eAcqOutputType acqOutputType2;
	int acqOutputType;

	/* ���ٴ����˲�ʹ�� */
	int bTrkWinFilter;

}configPlatParam;

/* ����ƽ̨���Ƴ�ʼ������ṹ */
typedef struct
{
	/* ��Ƶ���������� */
	SensorParams sensorParams[SENSOR_COUNT];
	/* �����豸���� */
	AnalogDeviceParam	analogDeviceParams[ANALOG_DEVICE_COUNT];
	/* �����豸���� */
	VirtualDeviceParam	virtualDeviceParams[VIRTUAL_DEVICE_COUNT];
	/* �豸�û����� */
	DeviceUserParam deviceUsrParam[DevUsr_MAX];
	/* �˲������� */
	PlatformFilterParam platformFilterParam[2][2];
	m_PlatformFilterParam m__cfg_platformFilterParam;
	/* �ֱ��ٶ�������� */
	JoystickRateDemandParam joystickRateDemandParam;

	/* ƽ̨��������ϵ�� */
	float scalarX;
	float scalarY;
	float demandMaxX;
	float demandMinX;
	float demandMaxY;
	float demandMinY;
	float deadbandX;
	float deadbandY;
	float driftX;
	float driftY;

	/* ƽ̨Bleedģʽ */
	int bleedUsed;
	float bleedX;
	float bleedY;

	/* ��ǰ��Ƶ������ */
	int iSensorInit;

	/* ��ǰ�ӳ� */
	float fFov[SENSOR_COUNT];

	/* ����ģʽƽ̨������� */
	eAcqOutputType acqOutputType2;
	int acqOutputType;

	/* ���ٴ����˲�ʹ�� */
	int bTrkWinFilter;

}PlatformCtrlParam;

typedef PlatformCtrlParam PlatformCtrl_CreateParams;
typedef configPlatParam	configPlatParam_InitParams;

/**
    ��������Ĭ�ϲ���

    ����pPrm������� ����

	���أ���
*/
__INLINE void PlatformCtrl_CreateParams_Init(PlatformCtrl_CreateParams *pPrm, configPlatParam_InitParams *m_Prm)
{
	int i;

	memset(pPrm, 0, sizeof(PlatformCtrl_CreateParams));

	for(i=0; i<SENSOR_COUNT; i++){
		SensorComp_CreateParams_Init(&pPrm->sensorParams[i]);
		pPrm->sensorParams[i].iIndex = i;
		pPrm->fFov[i] = pPrm->sensorParams[i].fFovMax;
	}
	pPrm->iSensorInit = 0;

	for(i=0; i<ANALOG_DEVICE_COUNT; i++){
		InputDevice_CreateParams_Init(&pPrm->analogDeviceParams[i]);
		pPrm->analogDeviceParams[i].iIndex = i;
	}
	for(i=0; i<VIRTUAL_DEVICE_COUNT; i++){
		InputDevice_CreateParams_Init(&pPrm->virtualDeviceParams[i]);
		pPrm->virtualDeviceParams[i].iIndex = i;
	}
	for(i=0; i<DevUsr_MAX; i++){
		DeviceUser_CreateParams_Init(&pPrm->deviceUsrParam[i]);
		pPrm->deviceUsrParam[i].iIndex = i;
		pPrm->deviceUsrParam[i].iInputSrcNum = i;
		pPrm->deviceUsrParam[i].inputSrcType = DEVUSER_InputSrcType_Virtual;
	}
	pPrm->deviceUsrParam[DevUsr_AcqWinSizeXInput].gainType = DEVUSER_GainType_Universal;
	pPrm->deviceUsrParam[DevUsr_AcqWinSizeYInput].gainType = DEVUSER_GainType_Universal;
	pPrm->deviceUsrParam[DevUsr_TrkWinSizeXInput].gainType = DEVUSER_GainType_Universal;
	pPrm->deviceUsrParam[DevUsr_TrkWinSizeYInput].gainType = DEVUSER_GainType_Universal;

	PlatformFilter_CreateParams_Init(&pPrm->platformFilterParam[0][0]);
	PlatformFilter_CreateParams_Init(&pPrm->platformFilterParam[0][1]);
	PlatformFilter_CreateParams_Init(&pPrm->platformFilterParam[1][0]);
	PlatformFilter_CreateParams_Init(&pPrm->platformFilterParam[1][1]);

	PlatformFilter_CreateParams_Gettxt(&pPrm->platformFilterParam[0][0],&pPrm->platformFilterParam[0][1], &m_Prm->m__cfg_platformFilterParam);

/*	pPrm->scalarX = 1/(100*PI/180.0*5.0*9/PI);
	pPrm->scalarY = 1/(400*PI/180.0*4.5*9/PI);
	pPrm->demandMaxX = 9999.0f;
	pPrm->demandMinX = -9999.0f;
	pPrm->demandMaxY = 9999.0f;
	pPrm->demandMinY = -9999.0f;

	pPrm->bleedUsed  = Bleed_BrosightError;
	pPrm->bleedX     =320;
	pPrm->bleedY     = 180;
	*/

	pPrm->scalarX = m_Prm->scalarX;
	pPrm->scalarY = m_Prm->scalarY;

	printf("X = %f\n", pPrm->scalarX);
	printf("Y = %f\n", pPrm->scalarY);

	pPrm->demandMaxX = m_Prm->demandMaxX;
	printf("platfromContorl===> demandMaxX = %f\n", pPrm->demandMaxX);
	pPrm->demandMinX = m_Prm->demandMinX;
	printf("platfromContorl===> demandMinX = %f\n", pPrm->demandMinX);
	pPrm->demandMaxY = m_Prm->demandMaxY;
	printf("platfromContorl===> demandMaxY = %f\n", pPrm->demandMaxY);
	pPrm->demandMinY = m_Prm->demandMinY;
	printf("platfromContorl===> demandMinY = %f\n", pPrm->demandMinY);

	pPrm->bleedUsed = m_Prm->bleedUsed;
	printf("platfromContorl===> bleedUsed = %d\n", pPrm->bleedUsed);
	pPrm->deadbandX = m_Prm->deadbandX;
	printf("platfromContorl===> deadbandX = %f\n", pPrm->deadbandX);
	pPrm->deadbandY = m_Prm->deadbandY;
	printf("platfromContorl===> deadbandY = %f\n", pPrm->deadbandY);
	pPrm->bleedX = m_Prm->bleedX;
	printf("platfromContorl===> bleedX = %f\n", pPrm->bleedX);
	pPrm->bleedY = m_Prm->bleedY;
	printf("platfromContorl===> bleedY = %f\n", pPrm->bleedY);

	pPrm->acqOutputType = m_Prm->acqOutputType;

	pPrm->joystickRateDemandParam.fCutpoint = 0.5f;
	pPrm->joystickRateDemandParam.fInputGain_X = 1.43;
	pPrm->joystickRateDemandParam.fInputGain_Y = 1.43;
	pPrm->joystickRateDemandParam.fDeadband = 0.15;
	pPrm->joystickRateDemandParam.fPlatformAcquisitionModeGain_X = 14.0f;
	pPrm->joystickRateDemandParam.fPlatformAcquisitionModeGain_Y = 12.0f;

	pPrm->bTrkWinFilter = 1;

}

/* ����ģ��ʵ��أ�ʵ���� */
HPLTCTRL PlatformCtrl_Create(PlatformCtrl_CreateParams *pPrm);

/* ���ģ��ʵ�� */
void PlatformCtrl_Delete(HPLTCTRL handle);

/* ��̬���ò��� */
int PlatformCtrl_SetParam(HPLTCTRL handle, PlatformCtrlParam *pParam);
int PlatformCtrl_SetSensorParam(HPLTCTRL handle, int iIndex, SensorParams *pParam);
int PlatformCtrl_SetVirtualDeviceParam(HPLTCTRL handle, int iIndex, VirtualDeviceParam *pParam);
int PlatformCtrl_SetDeviceUserParam(HPLTCTRL handle, int iIndex, DeviceUserParam *pParam);
int PlatformCtrl_SetPlatformFilterParam(HPLTCTRL handle, int iItem, int iSubItem, PlatformFilterParam *pParam);

/* ��̬��ȡ���� */
int PlatformCtrl_GetParam(HPLTCTRL handle, PlatformCtrlParam *pParam);
int PlatformCtrl_GetSensorParam(HPLTCTRL handle, int iIndex, SensorParams *pParam);
int PlatformCtrl_GetVirtualDeviceParam(HPLTCTRL handle, int iIndex, VirtualDeviceParam *pParam);
int PlatformCtrl_GetDeviceUserParam(HPLTCTRL handle, int iIndex, DeviceUserParam *pParam);
int PlatformCtrl_GetPlatformFilterParam(HPLTCTRL handle, int iItem, int iSubItem, PlatformFilterParam *pParam);
int PlatformCtrl_SetPlatformFilterParam(HPLTCTRL handle, int iItem, int iSubItem, PlatformFilterParam *pParam);

/* ��Ƶ���������ӳ��л� */
int PlatformCtrl_SensorInput(HPLTCTRL handle, int iSensor);
int PlatformCtrl_ZoomFovInput(HPLTCTRL handle, float fov);
int PlatformCtrl_SwitchFovInput(HPLTCTRL handle, int index);

/* ����ģ������ */
int PlatformCtrl_TrackerInput(HPLTCTRL handle, PLATFORMCTRL_TrackerInput *pInput);

/* ����ģ����� */
int PlatformCtrl_TrackerOutput(HPLTCTRL handle, PLATFORMCTRL_Output *pOutput);

/* ��õ�ǰ�豸����ֵ */
float PlatforCtrl_GetDevInputValue(void *pUser, eDEVUSER_InputSrcType type, int iNumber);

/* �����豸�û��������� */
int PlatformCtrl_LocalInput(HPLTCTRL handle, eDevUsr usr, float fValue);

/* �����豸�û�ģ������ */
int PlatformCtrl_AnalogInput(HPLTCTRL handle, int iIndex, float fValue);

/* �����豸�û��������� */
int PlatformCtrl_VirtualInput(HPLTCTRL handle, int iIndex, float fValue);

/* ���ò���ģʽƽ̨������� */
int PlatformCtrl_SelAcqPlatformOutput(HPLTCTRL handle, eAcqOutputType type);

int PlatformCtrl_outPutString(HPLTCTRL handle,char* str,int len);
#ifdef __cplusplus
}
#endif

#endif

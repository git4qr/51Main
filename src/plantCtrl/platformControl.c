#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "platformDebug.h"
#include "platformControl.h"
#include "Kalman.h"
#include <osa.h>

#define EPSINON   (0.000001f)

typedef struct
{
    HSENSORCOMP hSensor[SENSOR_COUNT];
    float fovX;//mRad
    float fovY;//mRad
    float width;//pix
    float height;//pix
    float curRateDemandX;
    float curRateDemandY;
    int lastPlatStat;
    HDEVUSR hDevUsers[DevUsr_MAX];
    float usrInputBak[DevUsr_MAX];

    HPLTFILTER filter[2][2];
    int iFilter;
    HPLTFILTER joystickIntegrator[2];
    PlatformFilterParam joystickIntegratorParam[2];
    HKalman hWinFilter;
    int iTrkAlgStateBak;
    eAcqOutputType acqOutputTypeBak;
} PLATFORMCTRL_pri;

typedef struct
{

    PLATFORMCTRL_Interface inter;
    PlatformCtrlParam params;
    PLATFORMCTRL_pri privates;
    PLATFORMDebug_obj trace;
} PlatformCtrl_Obj;


/* ����ģ��ʵ��أ�ʵ���� */
HPLTCTRL PlatformCtrl_Create(PlatformCtrl_CreateParams *pPrm)
{
    PlatformCtrl_Obj *pObj;
    int i;
    if(pPrm == NULL)
        return NULL;
    pObj = (PlatformCtrl_Obj *)malloc(sizeof(PlatformCtrl_Obj));
    if(pObj == NULL)
        return NULL;
    memset(pObj, 0, sizeof(PlatformCtrl_Obj));
    memcpy(&pObj->params, pPrm, sizeof(PlatformCtrl_CreateParams));
    pObj->inter.object = pObj;
    pObj->privates.iTrkAlgStateBak = 0;
    /* ������Ƶ������ */
    for(i = 0; i < SENSOR_COUNT; i++)
        pObj->privates.hSensor[i] = SensorComp_Create(&pObj->params.sensorParams[i]);

    pObj->inter.output.iSensor = pObj->params.iSensorInit;
    pObj->privates.fovX = pObj->params.fFov[pObj->inter.output.iSensor];
    pObj->privates.fovY = pObj->privates.fovX * pObj->params.sensorParams[pObj->inter.output.iSensor].fFovRatio;
    pObj->privates.width = (float)pObj->params.sensorParams[pObj->inter.output.iSensor].nWidth;
    pObj->privates.height = (float)pObj->params.sensorParams[pObj->inter.output.iSensor].nHeight;

    /* �����豸�û� */
    for(i = 0; i < DevUsr_MAX; i++)
        pObj->privates.hDevUsers[i] = DeviceUser_Create(&pObj->params.deviceUsrParam[i], pObj);

    /* �����˲��� */
    pObj->privates.filter[0][0] = PlatformFilter_Create(&pObj->params.platformFilterParam[0][0]);
    pObj->privates.filter[0][1] = PlatformFilter_Create(&pObj->params.platformFilterParam[0][1]);
    pObj->privates.filter[1][0] = PlatformFilter_Create(&pObj->params.platformFilterParam[1][0]);
    pObj->privates.filter[1][1] = PlatformFilter_Create(&pObj->params.platformFilterParam[1][1]);

    /* �����ֱ����� */
    pObj->privates.joystickIntegratorParam[0].P0
        = pObj->params.joystickRateDemandParam.fPlatformIntegratingGain_X;
    pObj->privates.joystickIntegratorParam[0].P1
        = (-1.0f) * pObj->params.joystickRateDemandParam.fPlatformIntegratingDecayGain;
    pObj->privates.joystickIntegrator[0]
        = PlatformFilter_Create(&pObj->privates.joystickIntegratorParam[0]);
    pObj->privates.joystickIntegratorParam[1].P0
        = pObj->params.joystickRateDemandParam.fPlatformIntegratingGain_Y;
    pObj->privates.joystickIntegratorParam[1].P1
        = (-1.0f) * pObj->params.joystickRateDemandParam.fPlatformIntegratingDecayGain;
    pObj->privates.joystickIntegrator[1]
        = PlatformFilter_Create(&pObj->privates.joystickIntegratorParam[1]);

    /* ���������˲��� */
    pObj->privates.hWinFilter = KalmanOpen(6, 3, 0);
    KalmanInitParam(pObj->privates.hWinFilter, 0.0, 0.0, 0, 0.0);

    /*�õ���ʼ�ӳ���������*/
    PlatformDebug_Init(&pObj->trace);
    return &pObj->inter;
}

/* ���ģ��ʵ�� */
void PlatformCtrl_Delete(HPLTCTRL handle)
{
    int i;
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
    if(pObj == NULL)
        return ;
    /* �ͷ��˲��� */
    PlatformFilter_Delete(pObj->privates.filter[0][0]);
    PlatformFilter_Delete(pObj->privates.filter[0][1]);
    PlatformFilter_Delete(pObj->privates.filter[1][0]);
    PlatformFilter_Delete(pObj->privates.filter[1][1]);
    /* �ͷ��ֱ����� */
    PlatformFilter_Delete(pObj->privates.joystickIntegrator[0]);
    PlatformFilter_Delete(pObj->privates.joystickIntegrator[1]);
    /* �ͷŴ����˲��� */
    KalmanClose(pObj->privates.hWinFilter);
    /* �ͷ��豸�û� */
    for(i = 0; i < DevUsr_MAX; i++)
        DeviceUser_Delete(pObj->privates.hDevUsers[i]);
    /* �ͷ���Ƶ������ */
    for(i = 0; i < SENSOR_COUNT; i++)
        SensorComp_Delete(pObj->privates.hSensor[i]);
    free(pObj);
}

/* ��̬���ò��� */
int PlatformCtrl_SetParam(HPLTCTRL handle, PlatformCtrlParam *pParam)
{
    int i;
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(&pObj->params, pParam, sizeof(PlatformCtrlParam));

    for(i = 0; i < DevUsr_MAX; i++)
        DeviceUser_SetParam(pObj->privates.hDevUsers[i], &pObj->params.deviceUsrParam[i]);

    return 0;
}
int PlatformCtrl_SetSensorParam(HPLTCTRL handle, int iIndex, SensorParams *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(&pObj->params.sensorParams[iIndex], pParam, sizeof(SensorParams));

    SensorComp_Delete(pObj->privates.hSensor[iIndex]);
    SensorComp_Create(&pObj->params.sensorParams[iIndex]);

    if(pObj->inter.output.iSensor == iIndex)
    {
        pObj->privates.fovX = pObj->params.fFov[pObj->inter.output.iSensor];
        pObj->privates.fovY = pObj->privates.fovX * pObj->params.sensorParams[pObj->inter.output.iSensor].fFovRatio;
        pObj->privates.width = (float)pObj->params.sensorParams[pObj->inter.output.iSensor].nWidth;
        pObj->privates.height = (float)pObj->params.sensorParams[pObj->inter.output.iSensor].nHeight;
    }

    return 0;
}
int PlatformCtrl_SetVirtualDeviceParam(HPLTCTRL handle, int iIndex, VirtualDeviceParam *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(&pObj->params.virtualDeviceParams[iIndex], pParam, sizeof(VirtualDeviceParam));
    return 0;
}
int PlatformCtrl_SetDeviceUserParam(HPLTCTRL handle, int iIndex, DeviceUserParam *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(&pObj->params.deviceUsrParam[iIndex], pParam, sizeof(DeviceUserParam));

    DeviceUser_SetParam(pObj->privates.hDevUsers[iIndex], &pObj->params.deviceUsrParam[iIndex]);

    return 0;
}
int PlatformCtrl_SetPlatformFilterParam(HPLTCTRL handle, int iItem, int iSubItem, PlatformFilterParam *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(&pObj->params.platformFilterParam[iItem][iSubItem], pParam, sizeof(PlatformFilterParam));
    return 0;
}
/* ��̬��ȡ���� */
int PlatformCtrl_GetParam(HPLTCTRL handle, PlatformCtrlParam *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(pParam, &pObj->params, sizeof(PlatformCtrlParam));
    return 0;
}
int PlatformCtrl_GetSensorParam(HPLTCTRL handle, int iIndex, SensorParams *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(pParam, &pObj->params.sensorParams[iIndex], sizeof(SensorParams));
    return 0;
}
int PlatformCtrl_GetVirtualDeviceParam(HPLTCTRL handle, int iIndex, VirtualDeviceParam *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(pParam, &pObj->params.virtualDeviceParams[iIndex], sizeof(VirtualDeviceParam));
    return 0;
}
int PlatformCtrl_GetDeviceUserParam(HPLTCTRL handle, int iIndex, DeviceUserParam *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(pParam, &pObj->params.deviceUsrParam[iIndex], sizeof(DeviceUserParam));
    return 0;
}
int PlatformCtrl_GetPlatformFilterParam(HPLTCTRL handle, int iItem, int iSubItem, PlatformFilterParam *pParam)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pParam == NULL)
        return -1;

    memcpy(pParam, &pObj->params.platformFilterParam[iItem][iSubItem], sizeof(PlatformFilterParam));
    return 0;
}

/* ��Ƶ�������л� */
int PlatformCtrl_SensorInput(HPLTCTRL handle, int iSensor)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL)
        return -1;

    pObj->inter.output.iSensor = iSensor;

    pObj->privates.width = (float)pObj->params.sensorParams[pObj->inter.output.iSensor].nWidth;
    pObj->privates.height = (float)pObj->params.sensorParams[pObj->inter.output.iSensor].nHeight;

    return 0;
}

/* �ӳ��л� */
int PlatformCtrl_ZoomFovInput(HPLTCTRL handle, float fZoom)
{
    float fovX = 0.0;

    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;
    int iSensor = pObj->inter.output.iSensor;

    if(pObj == NULL)
        return -1;

    fovX = pObj->params.sensorParams[iSensor].fFovMax * fZoom;
    pObj->privates.fovX = fovX;
    pObj->privates.fovY = pObj->params.sensorParams[iSensor].fFovRatio * pObj->privates.fovY / pObj->privates.fovX;

    return 0;
}
int PlatformCtrl_SwitchFovInput(HPLTCTRL handle, int index)
{
    return 0;
}
static int PlatformCtrl_PlatformCompensation(PlatformCtrl_Obj *pObj)
{
    float fTmp;

    fTmp = pObj->privates.curRateDemandX / pObj->params.scalarX;

    if(fTmp > pObj->params.demandMaxX)
        fTmp = pObj->params.demandMaxX;
    //printf("pObj->params.demandMaxX = %f\n", pObj->params.demandMaxX);

    if(fTmp < pObj->params.demandMinX)
        fTmp = pObj->params.demandMinX;
    //printf("pObj->params.demandMinX = %f\n", pObj->params.demandMinX);
// new
    if(fabsf(fTmp) < pObj->params.deadbandX)
    {
    	fTmp = 0;
    }
    printf("platformControl==========>deadbandX = %f\n", fTmp);

    fTmp += pObj->params.driftX;
    pObj->inter.output.fPlatformDemandX = fTmp;
    //printf("pObj->params.driftX = %f\n", pObj->params.driftX);


    fTmp = pObj->privates.curRateDemandY / pObj->params.scalarY;
    //printf("pObj->params.scalarY = %f\n", pObj->params.scalarY);

    if(fTmp > pObj->params.demandMaxY)
        fTmp = pObj->params.demandMaxY;
    //printf("pObj->params.demandMaxY = %f\n", pObj->params.demandMaxY);

    if(fTmp < pObj->params.demandMinY)
        fTmp = pObj->params.demandMinY;
    //printf("pObj->params.demandMinY = %f\n", pObj->params.demandMinY);

    // new
        if(fabsf(fTmp) < pObj->params.deadbandY)
        {
        	fTmp = 0;
        }
        printf("platformControl==========>deadbandX = %f\n", fTmp);

    fTmp += pObj->params.driftY;
    pObj->inter.output.fPlatformDemandY = fTmp;

    return 0;
}

static int PlatformCtrl_AcqRateDemand(PlatformCtrl_Obj *pObj)
{
    float fTmp;
    float fTmp2, K;
    JoystickRateDemandParam *pParam = &pObj->params.joystickRateDemandParam;

    pObj->privates.curRateDemandX = 0.0f;
    pObj->privates.curRateDemandY = 0.0f;

    switch(pObj->params.acqOutputType)
    {
        case AcqOutputType_Zero:
            break;

        case AcqOutputType_JoystickInput:
            pObj->privates.curRateDemandX = pObj->inter.devUsrInput[DevUsr_AcqJoystickXInput];
            pObj->privates.curRateDemandY = pObj->inter.devUsrInput[DevUsr_AcqJoystickYInput];
            break;

        case AcqOutputType_ShapedAndGained:
        case AcqOutputType_ShapedAndGainedAndIntegrated:
        {
            //X Axis
            fTmp = pObj->inter.devUsrInput[DevUsr_AcqJoystickXInput];
            //printf("INFO: devUsrInput[DevUsr_AcqJoystickXInput];: %f\r\n",fTmp);
            fTmp += pParam->fOffset_X;
#if 0

            if(fTmp > EPSINON)
                fTmp += pParam->fDeadband;

            if(fTmp < (-1)*EPSINON)
                fTmp -= pParam->fDeadband;

#else

            if(fabsf(fTmp) < pParam->fDeadband)
                fTmp = 0.0;

#endif

            if(fabs(fTmp) < pParam->fCutpoint)
            {
                fTmp *= pParam->fInputGain_X * 0.5f;
            }
            else
            {
                fTmp2 = pParam->fCutpoint * pParam->fInputGain_X * 0.5f;
                K = (pParam->fInputGain_X - fTmp2);
                K /= (1.0f - pParam->fCutpoint);
                fTmp = fTmp2 + (fTmp - pParam->fCutpoint) * K;
            }

            fTmp *= pParam->fPlatformAcquisitionModeGain_X;

            if(pObj->params.acqOutputType == AcqOutputType_ShapedAndGainedAndIntegrated)
            {
                if(pObj->privates.acqOutputTypeBak != pObj->params.acqOutputType)
                    PlatformFilter_Reset(pObj->privates.joystickIntegrator[0]);

                fTmp = PlatformFilter_Get(pObj->privates.joystickIntegrator[0], fTmp);
            }

            pObj->privates.curRateDemandX = fTmp;
           // printf("INFO: privates.curRateDemandX: %f\r\n",fTmp);

            // Y Axis
            fTmp = pObj->inter.devUsrInput[DevUsr_AcqJoystickYInput];
           // printf("INFO: YYYYYdevUsrInput[DevUsr_AcqJoystickYInput];: %f\r\n",fTmp);
            fTmp += pParam->fOffset_Y;
#if 0

            if(fTmp > EPSINON)
                fTmp += pParam->fDeadband;

            if(fTmp < (-1)*EPSINON)
                fTmp -= pParam->fDeadband;

#else

            if(fabsf(fTmp) < pParam->fDeadband)
                fTmp = 0.0;

#endif

            if(fabs(fTmp) < pParam->fCutpoint)
            {
                fTmp *= pParam->fInputGain_Y * 0.5f;
            }
            else
            {
                fTmp2 = pParam->fCutpoint * pParam->fInputGain_Y * 0.5f;
                K = (pParam->fInputGain_Y - fTmp2);
                K /= (1.0f - pParam->fCutpoint);
                fTmp = fTmp2 + (fTmp - pParam->fCutpoint) * K;
            }

            fTmp *= pParam->fPlatformAcquisitionModeGain_Y;

         //   printf("INFO: YYYprivates.curRateDemandY: %f\r\n",fTmp);

            if(pObj->params.acqOutputType == AcqOutputType_ShapedAndGainedAndIntegrated)
            {
                if(pObj->privates.acqOutputTypeBak != pObj->params.acqOutputType)
                    PlatformFilter_Reset(pObj->privates.joystickIntegrator[1]);

                fTmp = PlatformFilter_Get(pObj->privates.joystickIntegrator[1], fTmp);
            }

            pObj->privates.curRateDemandY = fTmp;
        }
            break;

        case AcqOutputType_DeterminedByPosition:
            break;

        case AcqOutputType_ZeroInitFilter:
            break;

        case AcqOutputType_DeterminedByIncomingPlatformData:
            break;

        default:
            break;
    }

    return 0;
}

static int PlatformCtrl_OutPlatformDemand(PlatformCtrl_Obj *pObj)
{
    PLATFORMCTRL_TrackerInput *pInput = &pObj->inter.trackerInput;
    float fTmp;
    float fTmpX;
    float fTmpY;

    if(pInput->iTrkAlgState == PlatStateType_Disable)//��ֹ
    {
        pObj->privates.curRateDemandX = 0.0f;
        pObj->privates.curRateDemandY = 0.0f;
    }

    if(pInput->iTrkAlgState == PlatStateType_Acquire)//����
    {
        PlatformCtrl_AcqRateDemand(pObj);

        if(pObj->privates.iTrkAlgStateBak != pInput->iTrkAlgState)//�˳�����
        {
            PlatformFilter_Reset(pObj->privates.filter[pObj->privates.iFilter][0]);
            PlatformFilter_Reset(pObj->privates.filter[pObj->privates.iFilter][1]);
        }
    }

    if(pInput->iTrkAlgState == PlatStateType_Trk_Lock)//��
    {
        fTmpX = pInput->fTargetBoresightErrorX;
        fTmpY = pInput->fTargetBoresightErrorY;
        if(pObj->params.bleedUsed == Bleed_BrosightError)
        {
            fTmpX = (fabs(fTmpX) > pObj->params.bleedX) ? pObj->params.bleedX*fabs(fTmpX)/fTmpX : fTmpX;
            fTmpY = (fabs(fTmpY) > pObj->params.bleedY) ? pObj->params.bleedY*fabs(fTmpY)/fTmpY : fTmpY;
        }

        fTmp = fTmpX * pObj->privates.fovX / pObj->privates.width;

        pObj->privates.curRateDemandX =
            PlatformFilter_Get(pObj->privates.filter[pObj->privates.iFilter][0], fTmp);

        fTmp = fTmpY * pObj->privates.fovY / pObj->privates.height;
        pObj->privates.curRateDemandY =
            PlatformFilter_Get(pObj->privates.filter[pObj->privates.iFilter][1], fTmp);
    }

    if(pInput->iTrkAlgState == PlatStateType_Trk_Break_Lock_1)//�ж���1
    {
		/*
		pObj->privates.curRateDemandX =
			PlatformFilter_Get(pObj->privates.filter[pObj->privates.iFilter][0], 0);

		pObj->privates.curRateDemandY =
			PlatformFilter_Get(pObj->privates.filter[pObj->privates.iFilter][1], 0);
		*/
    }

	if(pInput->iTrkAlgState == PlatStateType_Trk_Break_Lock_2)//�ж���1
	{
		pObj->privates.curRateDemandX = 0;
		pObj->privates.curRateDemandY = 0;
	}

    PlatformCtrl_PlatformCompensation(pObj);

	pObj->privates.lastPlatStat = pInput->iTrkAlgState;

    return 0;
}

/* �豸�û����봦�� */
static int PlatformCtrl_BuildDevUsrInput(PlatformCtrl_Obj *pObj)
{
    int i;

    for(i = 0; i < DevUsr_MAX; i++)
        pObj->inter.devUsrInput[i] = DeviceUser_Get(pObj->privates.hDevUsers[i]);

    return 0;
}

static int PlatformCtrl_ProccesDevUsrInput(PlatformCtrl_Obj *pObj)
{
    memcpy(pObj->privates.usrInputBak, pObj->inter.devUsrInput, sizeof(pObj->privates.usrInputBak));
    return 0;
}

/* ����ģ������ */
int PlatformCtrl_TrackerInput(HPLTCTRL handle, PLATFORMCTRL_TrackerInput *pInput)
{
    int iRet = 0;

    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pInput == NULL)
        return -1;

    memcpy(&pObj->inter.trackerInput, pInput, sizeof(PLATFORMCTRL_TrackerInput));

    PlatformCtrl_BuildDevUsrInput(pObj);

    PlatformCtrl_OutPlatformDemand(pObj);

    pObj->inter.output.iTrkAlgState = pInput->iTrkAlgState;
    pObj->inter.output.fBoresightPositionX = pInput->fBoresightPositionX;
    pObj->inter.output.fBoresightPositionY = pInput->fBoresightPositionY;
    pObj->inter.output.fTargetBoresightErrorX = pInput->fTargetBoresightErrorX;
    pObj->inter.output.fTargetBoresightErrorY = pInput->fTargetBoresightErrorY;

    if(pInput->iTrkAlgState == 0 || pInput->iTrkAlgState == 1)
    {
        pObj->inter.output.fWindowPositionX = pInput->fAcqWindowPositionX;
        pObj->inter.output.fWindowPositionY = pInput->fAcqWindowPositionY;
        pObj->inter.output.fWindowSizeX = pInput->fAcqWindowSizeX;
        pObj->inter.output.fWindowSizeY = pInput->fAcqWindowSizeY;

        if(pObj->privates.iTrkAlgStateBak > 1)
        {
            KalmanInitParam(pObj->privates.hWinFilter, 0.0, 0.0, 0, 0.0);
        }
    }
    else
    {
        pObj->inter.output.fWindowPositionX = pInput->fTargetBoresightErrorX;
        pObj->inter.output.fWindowPositionY = pInput->fTargetBoresightErrorY;
        pObj->inter.output.fWindowSizeX = pInput->fTargetSizeX;
        pObj->inter.output.fWindowSizeY = pInput->fTargetSizeY;

        if(pObj->params.bTrkWinFilter)
        {
            double dbM[3];
            dbM[0] = 0.0f;
            dbM[1] = pInput->fTargetBoresightErrorX;
            dbM[2] = pInput->fTargetBoresightErrorY;
            Kalman(pObj->privates.hWinFilter, dbM, NULL);

            pObj->inter.output.fWindowPositionX = (float)pObj->privates.hWinFilter->state_post[2];
            pObj->inter.output.fWindowPositionY = (float)pObj->privates.hWinFilter->state_post[4];
        }
    }

    PlatformCtrl_ProccesDevUsrInput(pObj);

    pObj->privates.iTrkAlgStateBak = pInput->iTrkAlgState;
    pObj->privates.acqOutputTypeBak = pObj->params.acqOutputType;

    return iRet;
}

/* ����ģ����� */
int PlatformCtrl_TrackerOutput(HPLTCTRL handle, PLATFORMCTRL_Output *pOutput)
{
    int iRet = 0;

    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL || pOutput == NULL)
        return -1;

    memcpy(pOutput, &pObj->inter.output, sizeof(PLATFORMCTRL_Output));

    return iRet;
}

/* ��õ�ǰ�豸����ֵ */
float PlatforCtrl_GetDevInputValue(void* handle, eDEVUSER_InputSrcType type, int iNumber)
{
    float fRet = 0.0f;
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle;

    if(pObj == NULL)
        return -1;

    switch(type)
    {
        case DEVUSER_InputSrcType_User:
            fRet = pObj->inter.localInput[iNumber];
            break;

        case DEVUSER_InputSrcType_Analog:
            fRet = pObj->inter.analogInput[iNumber];
            break;

        case DEVUSER_InputSrcType_Virtual:
            fRet = pObj->inter.virtalInput[iNumber];
            break;

        default:
            fRet = pObj->inter.localInput[iNumber];
            break;
    }

    return fRet;
}

/* �����豸�û��������� */
int PlatformCtrl_LocalInput(HPLTCTRL handle, eDevUsr usr, float fValue)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL)
        return -1;

    pObj->inter.localInput[usr] = fValue;

    return 0;
}

/* �����豸�û�ģ������ */
int PlatformCtrl_AnalogInput(HPLTCTRL handle, int iIndex, float fValue)
{
    float fTmp;
    AnalogDeviceParam *pParam;
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL)
        return -1;

    pParam = &pObj->params.analogDeviceParams[iIndex];

    fTmp = fValue;

    fTmp += pParam->fOffset;

    if(fTmp > EPSINON)
        fTmp += pParam->fDeadband;

    if(fTmp < (-1)*EPSINON)
        fTmp -= pParam->fDeadband;

    fTmp *= pParam->fGain;

    pObj->inter.analogInput[iIndex] = fTmp;

    return 0;
}

/* �����豸�û��������� */
int PlatformCtrl_VirtualInput(HPLTCTRL handle, int iIndex, float fValue)
{
    float fTmp;
    VirtualDeviceParam *pParam;
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL)
        return -1;

    pParam = &pObj->params.virtualDeviceParams[iIndex];

    fTmp = fValue;

    fTmp += pParam->fOffset;
#if 0

    if(fTmp > EPSINON)
        fTmp += pParam->fDeadband;

    if(fTmp < (-1)*EPSINON)
        fTmp -= pParam->fDeadband;

#else

    if(fabsf(fTmp) < pParam->fDeadband)
    {
        fTmp = 0.0;
    }

#endif
    fTmp *= pParam->fGain;
    pObj->inter.virtalInput[iIndex] = fTmp;

    return 0;
}

/* ���ò���ģʽƽ̨����������� */
int PlatformCtrl_SelAcqPlatformOutput(HPLTCTRL handle, eAcqOutputType type)
{
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL)
        return -1;

    pObj->params.acqOutputType = type;

    return 0;
}

int PlatformCtrl_outPutString(HPLTCTRL handle, char* str, int len)
{
    int retval = -1;
    PlatformCtrl_Obj *pObj = (PlatformCtrl_Obj*)handle->object;

    if(pObj == NULL)
        return retval;

    retval = PlatformDebug_Flush(&pObj->trace, str, len);

    return retval;
}

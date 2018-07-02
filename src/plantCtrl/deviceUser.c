#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "deviceUser.h"

#define EPSINON   (0.000001f)

typedef struct
{

    DeviceUser_CreateParams params;

    void *pUser;

    float curValue;

} DeviceUser_Obj;


extern float PlatforCtrl_GetDevInputValue(void * handle, eDEVUSER_InputSrcType type, int iNumber);

/* ���ģ���ʾ��ţ����أ�ģ���ʾ��� */
int DeviceUser_GetIndex(HDEVUSR hDevUsr)
{
    DeviceUser_Obj *pObj = (DeviceUser_Obj*)hDevUsr;

    if(pObj == NULL)
        return -1;

    return pObj->params.iIndex;
}

/* ����ģ��ʵ��أ�ʵ���� */
HDEVUSR DeviceUser_Create(DeviceUser_CreateParams *pPrm, void *pUser)
{
    DeviceUser_Obj *pObj;

    if(pPrm == NULL)
        return NULL;

    pObj = (DeviceUser_Obj *)malloc(sizeof(DeviceUser_Obj));

    if(pObj == NULL)
        return NULL;

    memset(pObj, 0, sizeof(DeviceUser_Obj));

    memcpy(&pObj->params, pPrm, sizeof(DeviceUser_CreateParams));

    pObj->pUser = pUser;

    return pObj;
}

/* ��̬���ò��� */
int DeviceUser_SetParam(HDEVUSR hDevUsr, DeviceUser_CreateParams *pPrm)
{
    DeviceUser_Obj *pObj = (DeviceUser_Obj*)hDevUsr;

    if(pObj == NULL)
        return -1;

    memcpy(&pObj->params, pPrm, sizeof(DeviceUser_CreateParams));

    return 0;
}

/* ���ģ��ʵ�� */
void DeviceUser_Delete(HDEVUSR hDevUsr)
{
    DeviceUser_Obj *pObj = (DeviceUser_Obj*)hDevUsr;

    if(pObj == NULL)
        return ;

    free(pObj);
}

/* ���� */
void DeviceUser_Reset(HDEVUSR hDevUsr)
{
    DeviceUser_Obj *pObj = (DeviceUser_Obj*)hDevUsr;

    if(pObj == NULL)
        return ;

    pObj->curValue = 0.0f;
}

/* ��õ�ǰ��� */
float DeviceUser_Get(HDEVUSR hDevUsr)
{
    float ret = 0.0;
    DeviceUser_Obj *pObj = (DeviceUser_Obj*)hDevUsr;

    if(pObj == NULL)
        return ret;

    ret = PlatforCtrl_GetDevInputValue(pObj->pUser, pObj->params.inputSrcType, pObj->params.iInputSrcNum);

    if(pObj->params.inputMethod == DEVUSER_InputMethod_Incremental)
    {
        if(pObj->params.gainType == DEVUSER_GainType_Incremental){
            pObj->curValue += ret * pObj->params.fGain;
        }
        else{
            pObj->curValue += ret;
        }
    }
    else
    {
        pObj->curValue = ret;
    }

    ret = pObj->curValue;

    if(pObj->params.gainType == DEVUSER_GainType_Universal)
    {
        ret *= pObj->params.fGain;
    }

    return ret;
}


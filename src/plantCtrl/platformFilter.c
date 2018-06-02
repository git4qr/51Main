#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "platformDebug.h"
#include "platformFilter.h"

#define EPSINON   (0.000001f)

typedef struct
{
    float Yc[3];
    float Xc[3];
} PlatformFilter_pri;

typedef struct
{

    PlatformFilter_CreateParams params;

    PlatformFilter_pri privates;

} PlatformFilter_Obj;




/* ���ģ���ʾ��ţ����أ�ģ���ʾ��� */
int PlatformFilter_GetIndex(HPLTFILTER hFilter)
{
    PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

    if(pObj == NULL)
        return -1;

    return pObj->params.iIndex;
}

/* ����ģ��ʵ��أ�ʵ���� */
HPLTFILTER PlatformFilter_Create(PlatformFilter_CreateParams *pPrm)
{
    PlatformFilter_Obj *pObj;

    if(pPrm == NULL)
        return NULL;

    pObj = (PlatformFilter_Obj *)malloc(sizeof(PlatformFilter_Obj));

    if(pObj == NULL)
        return NULL;

    memset(pObj, 0, sizeof(PlatformFilter_Obj));

    memcpy(&pObj->params, pPrm, sizeof(PlatformFilter_CreateParams));

    return pObj;
}

/* ���ģ��ʵ�� */
void PlatformFilter_Delete(HPLTFILTER hFilter)
{
    PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

    if(pObj == NULL)
        return ;

    free(pObj);
}

/* ���� */
void PlatformFilter_Reset(HPLTFILTER hFilter)
{
    PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

    if(pObj == NULL)
        return ;

    memset(&pObj->privates, 0, sizeof(pObj->privates));
}

/*
Yc(k) = G*[P0*Xc(k) + P1*Xc(k-1) + P2*Xc(k-2)] + [L1*Yc(k-1) + L2*Yc(k-2)]
Yc(k)   ���˲��������
Yc(k-1) ���˲�������ӳ�1�������
Yc(k-2) ���˲�������ӳ�2�������
Xc(k)   ����ǰ�������
Xc(k-1) ����������ӳ�1�������
Xc(k-2) ����������ӳ�2�������
*/
/* ��õ�ǰ��� */
float PlatformFilter_Get(HPLTFILTER hFilter, float curXc)
{
    float ret = 0.0;
    PlatformFilter_Obj *pObj = (PlatformFilter_Obj*)hFilter;

    if(pObj == NULL)
        return ret;

    //TRACE("%s curXc %f\n",__FUNCTION__,curXc);

    pObj->privates.Xc[2] = pObj->privates.Xc[1];
    pObj->privates.Xc[1] = pObj->privates.Xc[0];
    pObj->privates.Xc[0] = curXc;

    pObj->privates.Yc[2] = pObj->privates.Yc[1];
    pObj->privates.Yc[1] = pObj->privates.Yc[0];

    ret = pObj->params.G    \
          *(pObj->params.P0 * pObj->privates.Xc[0] + pObj->params.P1 * pObj->privates.Xc[1] + pObj->params.P2 * pObj->privates.Xc[2])
          + (pObj->params.L1 * pObj->privates.Yc[1] + pObj->params.L2 * pObj->privates.Yc[2]);

    pObj->privates.Yc[0] = ret;

    /*
    PlatformDebug_dump("Xc[0] %f Xc[1] %f Xc[2] %f",
        pObj->privates.Xc[0],pObj->privates.Xc[1],pObj->privates.Xc[2]);
    PlatformDebug_dump("Yc[0] %f Yc[1] %f Yc[2] %f",
        pObj->privates.Yc[0],pObj->privates.Yc[1],pObj->privates.Yc[2]);
    PlatformDebug_dump("curXc %f curRateDemandX %f",curXc,ret);
    */
    return ret;
}

void PlatformFilter_CreateParams_Set(
    PlatformFilter_CreateParams *pPrm, float P0, float P1, float P2, float L1, float L2, float gain)
{
    pPrm->P0 = P0;
    pPrm->P1 = P1;
    pPrm->P2 = P2;
    pPrm->L1 = L1;
    pPrm->L2 = L2;
    pPrm->G  = gain;
}

void PlatformFilter_CreateParams_Gettxt(PlatformFilter_CreateParams *pPrm,PlatformFilter_CreateParams *pPrm2, PlatformFilter_InitParams *m_pPrm)
{
#if 0
	FILE *fp=fopen("/home/ubuntu/ptz.txt","rb");
	if(fp==NULL){
			fprintf(stdout,"INFO: can not open ptz.txt\r\n");
	}
	char  * buffer=( char  *)malloc(100);
	fgets(buffer, 100, fp);
	float p0=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float p1=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float p2=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float l1=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float l2=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float gain=atof(buffer);
	memset(buffer,0,100);


	fgets(buffer, 100, fp);
	float p02=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float p12=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float p22=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float l12=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float l22=atof(buffer);
	memset(buffer,0,100);

	fgets(buffer, 100, fp);
	float gain2=atof(buffer);
	memset(buffer,0,100);
#endif
	pPrm->P0 = m_pPrm->P0;
	pPrm->P1 = m_pPrm->P1;
	pPrm->P2 = m_pPrm->P2;
	pPrm->L1 = m_pPrm->L1;
	pPrm->L2 = m_pPrm->L2;
	pPrm->G  = m_pPrm->G;

	pPrm2->P0 = m_pPrm->P02;
	pPrm2->P1 = m_pPrm->P12;
	pPrm2->P2 = m_pPrm->P22;
	pPrm2->L1 = m_pPrm->L12;
	pPrm2->L2 = m_pPrm->L22;
	pPrm2->G  = m_pPrm->G2;

	printf("xthe p0=%f  p1=%f p2=%f l1=%f l2=%f gain=%f\n",pPrm->P0,pPrm->P1,pPrm->P2,pPrm->L1,pPrm->L2,m_pPrm->G);
	printf("ythe p0=%f  p1=%f p2=%f l1=%f l2=%f gain=%f\n",pPrm2->P0,pPrm2->P1,pPrm2->P2,pPrm2->L1,pPrm2->L2,pPrm2->G);
	//free(buffer);


#if 0
    *P0 = pPrm->P0;
    *P1 = pPrm->P1;
    *P2 = pPrm->P2;
    *L1 = pPrm->L1;
    *L2 = pPrm->L2;
    *gain = pPrm->G;
#endif
}

void PlatformFilter_CreateParams_Get(
    PlatformFilter_CreateParams *pPrm, float *P0, float *P1, float *P2, float *L1, float *L2, float *gain)
{
    *P0 = pPrm->P0;
    *P1 = pPrm->P1;
    *P2 = pPrm->P2;
    *L1 = pPrm->L1;
    *L2 = pPrm->L2;
    *gain = pPrm->G;
}

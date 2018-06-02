#ifndef _PLATFORM_FILTER_H
#define _PLATFORM_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __INLINE
#    define __INLINE static __inline 
#endif

/* ����ƽ̨�˲������ */
typedef void * HPLTFILTER;

typedef struct
{
	int iIndex;

	float P0;

	float P1;

	float P2;

	float L1;

	float L2;

	float G;

	float P02;

	float P12;

	float P22;

	float L12;

	float L22;

	float G2;

}PlatformFilter_InitParams;

/* ����ƽ̨�˲�����ʼ������ṹ */
typedef struct
{
	/* ��ʾʵ����� */
	int iIndex;

	/*P0 Coefficient P0ϵ��*/
	float P0;
	
	/*P1 Coefficient P1ϵ��*/
	float P1;

	/*P2 Coefficient P2ϵ��*/
	float P2;

	/*l1 Coefficient l1ϵ��*/
	float L1;

	/*l2 Coefficient l2ϵ��*/
	float L2;

	/*Gain Coefficient ����ϵ��*/
	float G;

}PlatformFilter_CreateParams;

/*
Yc(k) = G*[P0*Xc(k) + P1*Xc(k-1) + P2*Xc(k-2)] + [L1*Yc(k-1) + L2*Yc(k-2)]
Yc(k)   ���˲��������
Yc(k-1) ���˲�������ӳ�1�������
Yc(k-2) ���˲�������ӳ�2�������
Xc(k)   ����ǰ�������
Xc(k-1) ����������ӳ�1�������
Xc(k-2) ����������ӳ�2�������
*/

/**
    ��������Ĭ�ϲ���

    ����pPrm������� ����

	���أ���
*/
__INLINE void PlatformFilter_CreateParams_Init(PlatformFilter_CreateParams *pPrm)
{
	memset(pPrm, 0, sizeof(PlatformFilter_CreateParams));
/*�������棩������*/
	pPrm->P0 = 1.0;
	pPrm->P1 = 0.0;
	pPrm->P2 = 0.0;
	pPrm->L1 = 0.0;
	pPrm->L2 = 0.0;
	pPrm->G  = 1.0;
#if 0
/*�����ֲ�����*/
	pPrm->P0 = (1.0f + 0.02f);
	pPrm->P1 = (-1.0f + 0.02f);
	pPrm->P2 = 0.0;
	pPrm->L1 = 1.0;
	pPrm->L2 = 0.0;
	pPrm->G = 1.0;
/*�����ֲ��������������0����*/
	pPrm->P0 = (1.0f + 0.01f);
	pPrm->P1 = (-1.0f + 0.01f);
	pPrm->P2 = 0.0;
	pPrm->L1 = 0.0;
	pPrm->L2 = 0.0;
	pPrm->G = 1.0;
/*������΢�ֲ�����*/
#endif
}

/* ���ģ���ʾ��ţ����أ�ģ���ʾ��� */
int PlatformFilter_GetIndex(HPLTFILTER hFilter);

/* ����ģ��ʵ��أ�ʵ���� */
HPLTFILTER PlatformFilter_Create(PlatformFilter_CreateParams *pPrm);

/* ���ģ��ʵ�� */
void PlatformFilter_Delete(HPLTFILTER hFilter);

/* ���� */
void PlatformFilter_Reset(HPLTFILTER hFilter);

/* ��õ�ǰ��� */
float PlatformFilter_Get(HPLTFILTER hFilter, float curValue);


void PlatformFilter_CreateParams_Set(
	PlatformFilter_CreateParams *pPrm,float P0,float P1,float P2,float L1, float L2, float gain);

void PlatformFilter_CreateParams_Get(
	PlatformFilter_CreateParams *pPrm,float *P0,float *P1,float *P2,float *L1, float *L2, float *gain);


void PlatformFilter_CreateParams_Gettxt(
    PlatformFilter_CreateParams *pPrm,PlatformFilter_CreateParams *pPrm2, PlatformFilter_InitParams *m_pPrm);
#ifdef __cplusplus
}
#endif

#endif

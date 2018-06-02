#ifndef _SENSORCOMPENSATE_H
#define _SENSORCOMPENSATE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __INLINE
#    define __INLINE static __inline 
#endif

/* ������ı��ʼ������С������С��2 */
#define SENSORCOMP_BORESIGHTPOS_TAB_DEF_SIZE		(2)

/* ������ı����ߴ� */
#define SENSORCOMP_BORESIGHTPOS_TAB_MAX_SIZE		(32)

/* �����ӳ�����ģ���� */
typedef void * HSENSORCOMP;

/* �����ӳ���������ģʽ */
typedef enum
{
	SENSORCOMP_MODE_RUN = 0, /**< ����ģʽ */
	SENSORCOMP_MODE_CORR_NORMAL, /**< У��ͨ��ģʽ */
	SENSORCOMP_MODE_CORR_CLEAN, /**< У�����ģʽ */
} SENSORCOMP_MODE;

/* �����ӳ����Ĳ����� */
typedef struct
{
	float fFov; //�ӳ���
	int x;      //����x���ֵ
	int y;      //����y���ֵ
}SensorComp_BoresightPos;


/* �����ӳ�������ʼ������ṹ */
typedef struct
{
	/* ��ʾʵ����� */
	int iIndex;

	/* ��������Ƶ��� */
	int nWidth;
	/* ��������Ƶ�߶� */
	int nHeight;

	/* Ĭ�ϰ���λ�� x */
	int defaultBoresightPos_x;
	/* Ĭ�ϰ���λ�� y */
	int defaultBoresightPos_y;

	/* ��С�ӳ��� */
	float fFovMin;
	/* ����ӳ��� */
	float fFovMax;

	/* FOV Ratio FOV���� ���崹ֱFOV/ˮƽFOV*/
	float fFovRatio;

	/* SensorComp_CreateParams_Init ΪNULLʱ��������Ĭ��Ϊ�� defaultBoresightPos_x��defaultBoresightPos_y*/
	/* ��ʼ����λ�ñ� */
	SensorComp_BoresightPos *initTab;
	/* ��ʼ����λ�ñ��С */
	int nInitTabSize;

	/* ��ʼģʽ */
	SENSORCOMP_MODE initMode;

}SensorComp_CreateParams;


/**
    ��������Ĭ�ϲ���

    ����pPrm������� ����

	���أ���
*/
__INLINE void SensorComp_CreateParams_Init(SensorComp_CreateParams *pPrm)
{
	memset(pPrm, 0, sizeof(SensorComp_CreateParams));
	pPrm->nWidth = 1920;
	pPrm->nHeight = 1080;
	pPrm->defaultBoresightPos_x = (pPrm->nWidth>>1);
	pPrm->defaultBoresightPos_y = (pPrm->nHeight>>1);
	pPrm->fFovMin = 3.0f;
	pPrm->fFovMax = 28;//20.0f;
	pPrm->fFovRatio = 0.75f;
	pPrm->initMode = SENSORCOMP_MODE_RUN;
}

/* ���ģ���ʾ��ţ����أ�ģ���ʾ��� */
int SensorComp_GetIndex(HSENSORCOMP hComp);

/* ����ģ��ʵ��أ�ʵ���� */
HSENSORCOMP SensorComp_Create(SensorComp_CreateParams *pPrm);

/* ���ģ��ʵ�� */
void SensorComp_Delete(HSENSORCOMP hComp);

/* ��õ�ǰ����ģʽ */
SENSORCOMP_MODE SensorComp_GetMode(HSENSORCOMP hComp);

/* �л���ǰ����ģʽ */
int SensorComp_SwitchMode(HSENSORCOMP hComp, SENSORCOMP_MODE mode, int flag);

/* ��ð���λ�ã����أ�0 �ɹ������� ʧ�� */
int SensorComp_GetBoresightPos(HSENSORCOMP hComp, float fFov, int *pX, int *pY);

/* ���ð���λ�ã�ֻ��У��ģʽ�µ��ã����أ�0 �ɹ������� ʧ��  */
int SensorComp_SetBoresightPos(HSENSORCOMP hComp, float fFov, int x, int y);

/* ��õ�ǰģʽ�µİ���λ�ñ?���أ�0 �ɹ������� ʧ��  */
int SensorComp_GetTab(HSENSORCOMP hComp, SensorComp_BoresightPos **ppTab, int *pSize);

/* ���ǰģʽ�µİ���λ�ñ?���أ�0 �ɹ������� ʧ��  */
int SensorComp_CleanTab(HSENSORCOMP hComp);

#ifdef __cplusplus
}
#endif

#endif

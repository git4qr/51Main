#ifndef _DEVICE_USER_H
#define _DEVICE_USER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __INLINE
#    define __INLINE static __inline 
#endif

/* 定义设备用户句柄 */
typedef void * HDEVUSR;

typedef enum {
	DEVUSER_InputSrcType_User = 0,
	DEVUSER_InputSrcType_Analog,
	DEVUSER_InputSrcType_Virtual
}eDEVUSER_InputSrcType;

typedef enum {
	DEVUSER_InputMethod_Absolute = 0,
	DEVUSER_InputMethod_Incremental
}eDEVUSER_InputMethod;

typedef enum {
	DEVUSER_GainType_Universal = 0,
	DEVUSER_GainType_Incremental
}eDEVUSER_GainType;

/* 定义设备用户初始化参数结构 */
typedef struct
{
	/* 标示实例序号 */
	int iIndex;

	/* 输入源类型 */
	eDEVUSER_InputSrcType inputSrcType;

	/* 输入源序号 */
	int iInputSrcNum;

	/* 输入方式 */
	eDEVUSER_InputMethod inputMethod;

	/* 增益系数 */
	float fGain;

	/* 增益类型 */
	eDEVUSER_GainType gainType;

}DeviceUser_CreateParams;

/**
    函数：设置默认参数

    参数：pPrm【输出】 参数

	返回：无
*/
__INLINE void DeviceUser_CreateParams_Init(DeviceUser_CreateParams *pPrm)
{
	memset(pPrm, 0, sizeof(DeviceUser_CreateParams));
	pPrm->fGain = 1.0f;
	pPrm->gainType = DEVUSER_GainType_Incremental;
}

/* 获得模块标示序号，返回：模块标示序号 */
int DeviceUser_GetIndex(HDEVUSR hDevUsr);

/* 创建模块实例，返回：实例句柄 */
HDEVUSR DeviceUser_Create(DeviceUser_CreateParams *pPrm, void *pUser);

/* 销毁模块实例 */
void DeviceUser_Delete(HDEVUSR hDevUsr);

/* 重置 */
void DeviceUser_Reset(HDEVUSR hDevUsr);

/* 获得当前值 */
float DeviceUser_Get(HDEVUSR hDevUsr);

/* 动态设置参数 */
int DeviceUser_SetParam(HDEVUSR hDevUsr, DeviceUser_CreateParams *pPrm);

#ifdef __cplusplus
}
#endif

#endif
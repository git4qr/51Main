#ifndef _INPUT_DEVICE_H
#define _INPUT_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __INLINE
#    define __INLINE static __inline 
#endif

/* 定义输入设备初始化参数结构 */
typedef struct
{
	/* 标示实例序号 */
	int iIndex;

	/* Deadband设备死区 */
	float fDeadband;
	
	/* Gain设备增益 */
	float fGain;

	/* Offset模拟设备偏置 */
	float fOffset;

}INPUTDEVICE_CreateParams;

typedef INPUTDEVICE_CreateParams VirtualDevice_CreateParams;
typedef INPUTDEVICE_CreateParams AnalogDevice_CreateParams;

/**
    函数：设置默认参数

    参数：pPrm【输出】 参数

	返回：无
*/
__INLINE void InputDevice_CreateParams_Init(VirtualDevice_CreateParams *pPrm)
{
	memset(pPrm, 0, sizeof(VirtualDevice_CreateParams));
	pPrm->fGain = 1.0f;
}

#ifdef __cplusplus
}
#endif

#endif
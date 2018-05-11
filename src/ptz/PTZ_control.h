#ifndef _PTZ_CONTROL_H
#define _PTZ_CONTROL_H

#include "osa.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include "osa_sem.h"
#include "port.hpp"
#include "pelcoProtocol.h"

typedef enum{
	PELCO_RESPONSE_Null = 0,
	PELCO_RESPONSE_General,
	PELCO_RESPONSE_Query,
	PELCO_RESPONSE_Extended
}PELCO_RESPONSE_t;

class CPTZControl
{
	port_handle *m_port;
    OSA_ThrHndl thrHandleDataIn;
    volatile Bool exitDataInThread;
    static Void * port_dataInFxn(Void * prm){
    	CPTZControl *pThis = (CPTZControl*)prm;
    	pThis->dataInThrd();
    };

    OSA_ThrHndl thrHandleMove;
    volatile Bool exitThreadMove;
    static Void * MoveThrdFxn(Void * prm){
    	CPTZControl *pThis = (CPTZControl*)prm;
    	struct timeval tmp;
    	tmp.tv_sec = 0;
    	tmp.tv_usec = 50000;
    	for(;pThis->exitThreadMove==FALSE;)
    	{
    		select(0, NULL, NULL, NULL, &tmp);
        	pThis->MoveSync();
        	pThis->MoveSpecialSync();
    	}
    	pThis->exitThreadMove = FALSE;
    };
public:
	CPTZControl();
	virtual ~CPTZControl();

	int Create();
	void Destroy();
	int SendCmd(LPPELCO_D_REQPKT pCmd, PELCO_RESPONSE_t tResp = PELCO_RESPONSE_General);
	int Move();
	int Preset(int nCtrlType, int iPresetNum);
	int Pattern(int nCtrlType, int iPatternNum);
	int Query(int iQueryType);
	int Dummy();
	int circle();

	unsigned char m_byAddr;
	int m_iSetPanSpeed, m_iSetTiltSpeed, m_iSetZoomSpeed,m_iSetIrisSpeed,m_iSetFocusNearSpeed, m_iSetFocusFarSpeed;

protected:

	unsigned char m_cmd1Code, m_cmd2Code, m_data1Code, m_data2Code;
	int m_iCurPanSpeed, m_iCurTiltSpeed, m_iCurZoomSpeed,m_iCurIrisSpeed,m_iCurFocusNearSpeed, m_iCurFocusFarSpeed;

	BOOL m_bChangePanSpeed, m_bChangeTiltSpeed, m_bChangeZoomSpeed,m_bChangeIrisSpeed,m_bChangeFocusNearSpeed, m_bChangeFocusFarSpeed;
	BOOL m_bStopZoom;
	int	m_iPanPos, m_iTiltPos, m_iZoomPos, m_iIrisPos,m_iMagnification, m_iFocusPos;
	BOOL m_bQuryZoomPos;
	float m_fZoomLimit;
	int m_circle;

protected:
	unsigned char recvBuffer[32];
	unsigned int uiCurRecvLen;
	unsigned int m_nWait;
	PELCO_RESPONSE_t m_tResponse;

	unsigned char sendBuffer[64];
	unsigned char sendBuffer1[64];
	LPPELCO_D_REQPKT m_pReq, m_pReqMove;
	LPPELCO_D_RESPPKT m_pResp;

	void dataInThrd();
	void RecvByte(unsigned char byRecv);
	int MoveSync();
	int MoveSpecialSync();

	OSA_MutexHndl m_mutex;
	OSA_SemHndl m_sem;
};

#if 0

typedef void (*PTZNotifyRtn)(void *, unsigned char *, unsigned int);

typedef enum{
	PELCO_RESPONSE_Null = 0,
	PELCO_RESPONSE_General,
	PELCO_RESPONSE_Query,
	PELCO_RESPONSE_Extended
}PELCO_RESPONSE_t;

class CQTimer ;
class CPTZControl 
{
public:
	CPTZControl();
	virtual ~CPTZControl();
	CSerialPort   m_serial;
	SERIALPORT_PARAM m_serialParam;

	BOOL m_bOpen;

	unsigned char *sendBuffer;
	unsigned char *sendBuffer1;
	unsigned char *recvBuffer;
	unsigned int uiCurSendLen;
	unsigned int uiCurRecvLen;
	LPPELCO_D_REQPKT m_pReq, m_pReqMove;
	LPPELCO_D_RESPPKT m_pResp;

	HANDLE m_hEvent;
	unsigned int m_nWait;
	PELCO_RESPONSE_t m_tResponse;

	CExLock m_lock;

	unsigned char m_byAddr;

	BYTE m_cmd1Code, m_cmd2Code, m_data1Code, m_data2Code;
	int m_iCurPanSpeed, m_iCurTiltSpeed, m_iCurZoomSpeed,m_iCurIrisSpeed;
	int m_iSetPanSpeed, m_iSetTiltSpeed, m_iSetZoomSpeed,m_iSetIrisSpeed;
	BOOL m_bChangePanSpeed, m_bChangeTiltSpeed, m_bChangeZoomSpeed,m_bChangeIrisSpeed;
	BOOL m_bStopZoom;

	int	m_iPanPos, m_iTiltPos, m_iZoomPos, m_iIrisPos,m_iMagnification;

	BOOL m_bQuryZoomPos;
	float m_fZoomLimit;

	int m_circle;
public:
	int Create();
	void Destroy();

	int SendCmd(LPPELCO_D_REQPKT pCmd, PELCO_RESPONSE_t tResp = PELCO_RESPONSE_General);

	int Move();
	int Preset(int nCtrlType, int iPresetNum);
	int Pattern(int nCtrlType, int iPatternNum);
	int Query(int iQueryType);
	int Dummy();

	int circle();
protected:

	int MoveSync();

	// thread
	HANDLE			m_hThread;
	HANDLE			m_hEventThread[2];
	void PtzThread(void);
	static DWORD ThreadRoutine(CPTZControl *pControl){
		pControl->PtzThread();
		return 0;
	}

	BOOL m_bTimerRun;
	CQTimer *m_pQTimer;
	void TimerHandle();
	void RecvByte(BYTE byRecv);
	static void SerialRtn(CPTZControl *pControl, int port, int iCommEvent, int param){
		if( iCommEvent & EV_RXCHAR ){
			pControl->RecvByte((BYTE)param);
		}else{
			TRACE("Serial %d Event: %04X\n", port, iCommEvent);
		}
	}
	static void CALLBACK TimerHandle_(CPTZControl *control, BOOL TimerOrWaitFired){
		control->TimerHandle();
	}

	//���Ƹ�����˵�
public:
	void Menu();
	void MenuEnter();
	void MenuUp();
	void MenuDown();
};
#endif


#endif

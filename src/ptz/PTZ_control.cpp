#include "pelcoProtocol.h"
#include "PTZ_control.h"

#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#define DATAIN_TSK_PRI              (2)
#define DATAIN_TSK_STACK_SIZE       (0)

CPTZControl::CPTZControl():m_port(NULL)
{
	m_byAddr = 1;
	m_bStopZoom = FALSE;
	m_circle = 0;

	m_iCurPanSpeed = m_iCurTiltSpeed = m_iCurZoomSpeed = m_iCurIrisSpeed = m_iCurFocusNearSpeed = m_iCurFocusFarSpeed = m_iSetPreset = 0;
	m_iSetPanSpeed = m_iSetTiltSpeed = m_iSetZoomSpeed = m_iSetIrisSpeed =m_iSetFocusNearSpeed =  m_iSetFocusFarSpeed = m_iSetPreset =  0;
	m_bChangePanSpeed = m_bChangeTiltSpeed = m_bChangeZoomSpeed = m_bChangeIrisSpeed =
	m_bChangeFocusNearSpeed = m_bChangeFocusFarSpeed = m_bChangePreset = FALSE;
	m_iPanPos = m_iTiltPos = m_iZoomPos = m_iIrisPos= m_iFocusPos = 0;

	m_bQuryZoomPos = FALSE;
	m_fZoomLimit = 32.0;

	m_cmd1Code = 0;
	m_cmd2Code = 0;
	m_data1Code = 0;
	m_data2Code = 0;

}

CPTZControl::~CPTZControl()
{
	Destroy();
}

int CPTZControl::Create()
{
	int iRet = OSA_SOK;

	m_pReq = (LPPELCO_D_REQPKT)sendBuffer;
	m_pReqMove = (LPPELCO_D_REQPKT)sendBuffer1;
	m_pResp = (LPPELCO_D_RESPPKT)recvBuffer;

    uart_open_params	mUartOpenParams;
    strcpy( mUartOpenParams.device, "/dev/ttyTHS2" );
    mUartOpenParams.baudrate   = 4800;//4800;
    mUartOpenParams.databits   = 8;
    mUartOpenParams.parity	   = 'N';
    mUartOpenParams.stopbits   = 1;

    //OSA_printf("%s: port create call... ", __func__);

	iRet = port_create(PORT_UART, &m_port);
    OSA_assert( iRet == OSA_SOK );
    OSA_assert( m_port != NULL );
    //OSA_printf("%s: port open [%p]call... ", __func__, m_port->open);
    OSA_assert(m_port->open != NULL);
    iRet = m_port->open( m_port, &mUartOpenParams );
    printf("PTZ======>create is OK!\n");
    OSA_assert( iRet == OSA_SOK );

    OSA_mutexCreate(&m_mutex);
    OSA_semCreate(&m_sem, 1, 0);

	uiCurRecvLen = 0;
	m_tResponse =PELCO_RESPONSE_General;// PELCO_RESPONSE_Null;
	m_nWait = 0;
    exitDataInThread = FALSE;
    //OSA_printf("%s: thrHandleDataIn create call... \n", __func__);
    OSA_thrCreate(&thrHandleDataIn, port_dataInFxn,  DATAIN_TSK_PRI, DATAIN_TSK_STACK_SIZE, this);

    exitThreadMove = FALSE;
    OSA_thrCreate(&thrHandleMove,MoveThrdFxn, DATAIN_TSK_PRI, DATAIN_TSK_STACK_SIZE, this);

    //OSA_printf("%s: finish %d ", __func__, iRet);

	return iRet;
}

void CPTZControl::Destroy()
{
	if(m_port != NULL){
		OSA_mutexLock(&m_mutex);
		exitDataInThread = TRUE;
		do{
			OSA_waitMsecs(5);
		}while(exitDataInThread);
		OSA_thrDelete(&thrHandleDataIn);

		exitThreadMove = TRUE;
		do{
			OSA_waitMsecs(5);
		}while(exitThreadMove);
		OSA_thrDelete(&thrHandleMove);

		port_destory(m_port);
		m_port = NULL;

		OSA_mutexUnlock(&m_mutex);
		OSA_mutexDelete(&m_mutex);
		OSA_semSignal(&m_sem);
		OSA_semDelete(&m_sem);
	}
}

void CPTZControl::dataInThrd()
{
    Int32 result;
    fd_set rd_set;
    Uint8 *buffer = NULL;
    Int32 rlen;
    Int32 i;
    struct timeval timeout;

    memset(&timeout, 0, sizeof(timeout));

    buffer = (Uint8*)malloc(1024);

    OSA_assert(buffer != NULL);

    OSA_printf(" %d: %s   : run  !!!\n",
               OSA_getCurTimeInMsec(), __func__);

    while(!exitDataInThread && m_port != NULL && m_port->fd != -1)
    {
        FD_ZERO(&rd_set);
        FD_SET(m_port->fd, &rd_set);
        timeout.tv_sec   = 0;
        timeout.tv_usec = 200000;
        result = select(m_port->fd+1, &rd_set, NULL, NULL, &timeout);
        //result = select(0, NULL, NULL, NULL, &timeout);
        if(result == -1 || exitDataInThread )
            break;

        if(result == 0)
        {
            //OSA_waitMsecs(1);
        	//OSA_printf("%s: timeout.", __func__);
            continue;
        }

        if(FD_ISSET(m_port->fd, &rd_set))
        {
            rlen =  m_port->recv(m_port, buffer, 1024);
            OSA_assert(rlen > 0);
            for(i=0; i<rlen; i++)
            	RecvByte(buffer[i]);
          //  OSA_printf("%s: rlen = %d. [%02x %02x %02x %02x %02x %02x %02x]",__func__, rlen, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        }
    }
    free(buffer);

    OSA_printf(" %d: %s   : exit !!!\n",
               OSA_getCurTimeInMsec(), __func__);
    exitDataInThread = FALSE;
}

void CPTZControl::RecvByte(unsigned char byRecv)
{
	if(uiCurRecvLen == 0){

		if(byRecv == 0xFF){
			if(m_tResponse == PELCO_RESPONSE_Null)
				return ;
			recvBuffer[uiCurRecvLen++] = byRecv;
			if(m_tResponse == PELCO_RESPONSE_Query)
				m_nWait = 18;
			else if(m_tResponse == PELCO_RESPONSE_Extended)
				m_nWait = 7;
			else
				m_nWait = 4;
		}
	}else{
		recvBuffer[uiCurRecvLen++] = byRecv;
		if(uiCurRecvLen == m_nWait){
			if(recvBuffer[3] != 0x59 && recvBuffer[3] != 0x5B
				&& recvBuffer[3] != 0x5D && recvBuffer[3] != 0x63){
				OSA_printf("PTZ R> %d %02X %02X %02X %02X %02X %02X %02X\n", uiCurRecvLen,
					recvBuffer[0], recvBuffer[1], recvBuffer[2], recvBuffer[3],
					recvBuffer[4], recvBuffer[5], recvBuffer[6]);
			}

			switch(recvBuffer[3])
			{
			case 0x59:
				m_iPanPos = recvBuffer[4];
				m_iPanPos <<= 8;
				m_iPanPos += recvBuffer[5];
				//fprintf(stdout, "INFO: m_iPanPos is %d",m_iPanPos);
				break;
			case 0x5B:
				m_iTiltPos = recvBuffer[4];
				m_iTiltPos <<= 8;
				m_iTiltPos += recvBuffer[5];
				//printf("INFO: m_iTiltPos is %d",m_iTiltPos);
				break;
			case 0x5D:
				m_iZoomPos = recvBuffer[4];
				m_iZoomPos <<= 8;
				m_iZoomPos += recvBuffer[5];
			//	printf("INFO: zoompos is %d",m_iZoomPos);
				break;
			case 0x63:
				m_iMagnification = recvBuffer[4];
				m_iMagnification <<= 8;
				m_iMagnification += recvBuffer[5];
				break;
			}

			if(m_tResponse != PELCO_RESPONSE_Null)
				OSA_semSignal(&m_sem);
			uiCurRecvLen = 0;
			m_nWait = 0;
		}
	}
}


int CPTZControl::SendCmd(LPPELCO_D_REQPKT pCmd, PELCO_RESPONSE_t tResp /* = PELCO_RESPONSE_General */)
{
	int iRet = OSA_SOK;
	unsigned char *buffer = (unsigned char *)pCmd;

	OSA_mutexLock(&m_mutex);
	m_tResponse = tResp;
	if(m_port == NULL || m_port->send(m_port, (unsigned char *)pCmd, sizeof(PELCO_D_REQPKT)) != sizeof(PELCO_D_REQPKT))
	{
		OSA_printf("%s: send error!", __func__);
		OSA_mutexUnlock(&m_mutex);
		return OSA_EFAIL;
	}

    //OSA_printf("%s: [%02x %02x %02x %02x %02x %02x %02x]",__func__, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);


	if(tResp != PELCO_RESPONSE_Null)
		iRet = OSA_semWait(&m_sem, 200);
	/*else
		WaitForSingleObject(m_hEvent, 10);*/
	if( iRet != OSA_SOK ){
		OSA_printf("PTZ send msg time out!\n");
		iRet = -1;
	}
	OSA_mutexUnlock(&m_mutex);

	return iRet;
}

int CPTZControl::MovePreset()
{
	int iRet = 0;
	OSA_mutexLock(&m_mutex);
	m_cmd1Code = 0;
	if(m_bChangePreset){
		if(m_iCurPreset > 0){
			int type = 0x07;
			int value = 0x01;
			//Preset(type, value);
		}
		m_bChangePreset = false;
		printf("Preset success!!!\n");
	}
	OSA_mutexUnlock(&m_mutex);

	return iRet;
}

int CPTZControl::MoveSpecialSync()
{
	int iRet = 0;
	OSA_mutexLock(&m_mutex);
	m_cmd1Code = 0;
	if(m_bChangeFocusFarSpeed ||m_bChangeIrisSpeed){

		if(m_iCurFocusFarSpeed < 0){
			m_cmd1Code |= 0x01;
			}

		if(m_iCurIrisSpeed > 0){	//small
			m_cmd1Code |= 0x02;
		}else if(m_iCurIrisSpeed <0){
			m_cmd1Code |= 0x04;
		}
		m_cmd2Code = 0;
		m_data1Code = 0;
		m_data2Code = 0;


		IPelcoDFormat::PktFormat(m_pReqMove, m_cmd1Code, m_cmd2Code, m_data1Code, m_data2Code, m_byAddr);
		OSA_mutexUnlock(&m_mutex);
		iRet = SendCmd(m_pReqMove, PELCO_RESPONSE_Null);
		//fprintf(stdout, "send special success!\n");
		OSA_mutexLock(&m_mutex);
		m_bChangeFocusFarSpeed =  m_bChangeIrisSpeed = false;
	}
	OSA_mutexUnlock(&m_mutex);
	return iRet;
}
int CPTZControl::MoveSync()
{
	int iRet = 0;

	OSA_mutexLock(&m_mutex);

	if(m_bChangeZoomSpeed || m_bChangeTiltSpeed || m_bChangePanSpeed||m_bChangeFocusNearSpeed){
		int Pan  = abs((int)(m_iCurPanSpeed));
		int Tilt = abs((int)m_iCurTiltSpeed);
		int Zoom = abs((int)m_iCurZoomSpeed);
		int Iris = abs((int)m_iCurIrisSpeed);
		int FocusNear = abs((int)m_iCurFocusNearSpeed);

		m_cmd1Code=0;
		m_cmd2Code = 0;


				if(m_iCurFocusNearSpeed > 0){
					m_cmd2Code |= 0x80;
					//printf("This is Focus Near\n");
							}

				if(m_iCurPanSpeed>0)
					m_cmd2Code |= 0x02;
				else if(m_iCurPanSpeed<0)
					m_cmd2Code |= 0x04;

				if(m_iCurTiltSpeed > 0)
					m_cmd2Code |= 0x10;
				else if(m_iCurTiltSpeed < 0)
					m_cmd2Code |= 0x08;

				if(m_iCurZoomSpeed > 0){		//Tele
					m_cmd2Code |= 0x20;
					m_bStopZoom = TRUE;
				}else if(m_iCurZoomSpeed <0){//Wide
					m_cmd2Code |= 0x40;
					m_bStopZoom = TRUE;
				}
				/*else{
					if(m_bStopZoom){
						//m_pQTimer->SetQTimer(50, (fpWAITORTIMERCALLBACK)TimerHandle_, this);
						//TRACE("STOP!\n");
					}
					m_bStopZoom = FALSE;
				}
				*/

				m_data1Code = min(Pan, 63);
				m_data2Code = min(Tilt, 63);



				IPelcoDFormat::PktFormat(m_pReqMove, m_cmd1Code, m_cmd2Code, m_data1Code, m_data2Code, m_byAddr);
				OSA_mutexUnlock(&m_mutex);
				iRet = SendCmd(m_pReqMove, PELCO_RESPONSE_Null);
				//fprintf(stdout, "send success!\n");
				OSA_mutexLock(&m_mutex);
		m_bChangePanSpeed = m_bChangeTiltSpeed = m_bChangeZoomSpeed = m_bChangeFocusNearSpeed = FALSE;
	}

	OSA_mutexUnlock(&m_mutex);

	return iRet;
}

int CPTZControl::Move()
{
	int iRet = 0;

	OSA_mutexLock(&m_mutex);

	if(m_iCurPanSpeed == m_iSetPanSpeed
		&& m_iCurTiltSpeed == m_iSetTiltSpeed
		&& m_iCurZoomSpeed == m_iSetZoomSpeed
		&& m_iCurIrisSpeed == m_iSetIrisSpeed
		&& m_iCurFocusNearSpeed == m_iSetFocusNearSpeed
		&&m_iCurFocusFarSpeed == m_iSetFocusFarSpeed
		&&m_iCurPreset == m_iSetPreset)
	{
		//TRACE("Speed no change !\n");
		OSA_mutexUnlock(&m_mutex);
		return 0;
	}
	if(m_iSetZoomSpeed > 10)
		m_iSetZoomSpeed = 10;
	if(m_iSetZoomSpeed < -10)
		m_iSetZoomSpeed = -10;
	m_bChangeZoomSpeed = ((int)m_iCurZoomSpeed != (int)m_iSetZoomSpeed);
	m_bChangePanSpeed = ((int)m_iCurPanSpeed != (int)m_iSetPanSpeed);
	m_bChangeTiltSpeed = ((int)m_iCurTiltSpeed != (int)m_iSetTiltSpeed);
	m_bChangeIrisSpeed =((int)m_iCurIrisSpeed != (int)m_iSetIrisSpeed);
	m_bChangeFocusNearSpeed =((int)m_iCurFocusNearSpeed != (int)m_iSetFocusNearSpeed);
	m_bChangeFocusFarSpeed =((int)m_iCurFocusFarSpeed != (int)m_iSetFocusFarSpeed);
	m_bChangePreset = ((int)m_iCurPreset != (int)m_iSetPreset);
	m_iCurPanSpeed = m_iSetPanSpeed;
	m_iCurTiltSpeed = m_iSetTiltSpeed;
	m_iCurZoomSpeed = m_iSetZoomSpeed;
	m_iCurIrisSpeed = m_iSetIrisSpeed;
	m_iCurFocusFarSpeed = m_iSetFocusFarSpeed;
	m_iCurFocusNearSpeed = m_iSetFocusNearSpeed;
	m_iCurPreset = m_iSetPreset;

	OSA_mutexUnlock(&m_mutex);

	return 0;
}

int CPTZControl::circle()
{
	Preset(PTZ_PRESET_GOTO, m_circle);
	m_circle = (m_circle+1)%5;
	return m_circle;
}

int CPTZControl::Preset(int nCtrlType, int iPresetNum)
{
	IPelcoDFormat::MakePresetCtrl(m_pReq, nCtrlType, iPresetNum, m_byAddr);
	return SendCmd(m_pReq, PELCO_RESPONSE_General);
}

int CPTZControl::Pattern(int nCtrlType, int iPatternNum)
{
	IPelcoDFormat::MakePatternCtrl(m_pReq, nCtrlType, iPatternNum, m_byAddr);
	OSA_printf("Pattern type = %d\n", nCtrlType);
	return SendCmd(m_pReq, PELCO_RESPONSE_General);
}

int CPTZControl::Query(int iQueryType)
{
	IPelcoDFormat::MakeExtCommand(m_pReq, iQueryType, 0, 0, m_byAddr);
	return SendCmd(m_pReq, PELCO_RESPONSE_Extended);
}

int CPTZControl::Dummy()
{
	IPelcoDFormat::MakeDummy(m_pReq, m_byAddr);
	return SendCmd(m_pReq, PELCO_RESPONSE_Null);
}


#if 0
CPTZControl::CPTZControl()
{
	m_hThread = NULL;
	m_hEventThread[0] = NULL;
	m_hEventThread[1] = NULL;

	uiCurSendLen = 0;
	uiCurRecvLen = 0;
	sendBuffer = NULL;
	recvBuffer = NULL;
	m_pQTimer = NULL;
	m_pReq = NULL;
	m_pResp = NULL;
	m_hEvent = NULL;
	m_nWait = 0;
	m_tResponse = PELCO_RESPONSE_Null;
	m_byAddr = 1;
	m_bOpen = FALSE;
	m_bTimerRun = FALSE;
	m_bStopZoom = FALSE;
	
	m_circle = 0;

	m_iCurPanSpeed = m_iCurTiltSpeed = m_iCurZoomSpeed = m_iCurIrisSpeed = 0;
	m_iSetPanSpeed = m_iSetTiltSpeed = m_iSetZoomSpeed = m_iSetIrisSpeed = 0;
	m_bChangePanSpeed = m_bChangeTiltSpeed = m_bChangeZoomSpeed = m_bChangeIrisSpeed = FALSE;
	m_iPanPos = m_iTiltPos = m_iZoomPos = m_iIrisPos=0;

	m_bQuryZoomPos = FALSE;
	m_fZoomLimit = 32.0;

	m_cmd1Code = 0;
	m_cmd2Code = 0;
	m_data1Code = 0;
	m_data2Code = 0;

	m_serialParam.portNum = 6;
	m_serialParam.baud = 2400;
	m_serialParam.parity = 'N';
	m_serialParam.databits = 8;
	m_serialParam.stopbits = 1;
}

CPTZControl::~CPTZControl()
{
	Destroy();
}

int CPTZControl::Create()
{
	sendBuffer = (unsigned char *)malloc(1024);
	sendBuffer1 = (unsigned char *)malloc(1024);
	recvBuffer = (unsigned char *)malloc(1024);
	memset(sendBuffer, 0, 1024);
	memset(sendBuffer1, 0, 1024);
	memset(recvBuffer, 0, 1024);
	m_hEvent = CreateEvent(NULL , FALSE , FALSE , NULL);
	m_pReq = (LPPELCO_D_REQPKT)sendBuffer;
	m_pReqMove = (LPPELCO_D_REQPKT)sendBuffer1;
	m_pResp = (LPPELCO_D_RESPPKT)recvBuffer;

	if(!m_serial.InitPort(this, (SerialPortRtn)SerialRtn,
		m_serialParam.portNum,
		m_serialParam.baud, 
		m_serialParam.parity,
		m_serialParam.databits,
		m_serialParam.stopbits,
		EV_RXFLAG | EV_RXCHAR | EV_TXEMPTY,512)){
		return -1;
	}else{
		m_serial.StartMonitoring();
		m_bOpen = TRUE;
	}	

	m_pQTimer = new CQTimer("PTZ");

	m_hEventThread[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventThread[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD dwThreadId;
	ResetEvent(m_hEventThread[0]);
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ThreadRoutine, this, 0, &dwThreadId);

  	IPelcoDFormat::MakeExtCommand(m_pReq, 0x25, 0/*Zoom*/, 0, m_byAddr);
	SendCmd(m_pReq, PELCO_RESPONSE_General);
	IPelcoDFormat::MakeExtCommand(m_pReq, PTZ_QUERY_Pan, 0, 0, m_byAddr);
	SendCmd(m_pReq, PELCO_RESPONSE_Extended);
	IPelcoDFormat::MakeExtCommand(m_pReq, PTZ_QUERY_Tilt, 0, 0, m_byAddr);
	SendCmd(m_pReq, PELCO_RESPONSE_Extended);
	IPelcoDFormat::MakeExtCommand(m_pReq, PTZ_QUERY_Zoom, 0, 0, m_byAddr);
	SendCmd(m_pReq, PELCO_RESPONSE_Extended);

	return 0;
}

void CPTZControl::Destroy()
{
	m_lock.Lock();
	m_bOpen = FALSE;
	if(m_pQTimer != NULL){
		m_pQTimer->KillQTimer();
		delete m_pQTimer;
		m_pQTimer = NULL;
	}
	m_lock.Unlock();
	
	if( m_hThread != NULL ){
		SetEvent(m_hEventThread[0]);
		if(WaitForSingleObject(m_hThread, INFINITE) != WAIT_OBJECT_0){
			TerminateThread(m_hThread, 1);
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}	

	if( m_hEventThread[0] != NULL )
		CloseHandle(m_hEventThread[0]);
	if( m_hEventThread[1] != NULL )
		CloseHandle(m_hEventThread[1]);
	m_hEventThread[0] = NULL;
	m_hEventThread[1] = NULL;

	m_serial.UnitPort();

	if( m_hEvent != NULL ){
		SetEvent(m_hEvent);
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	if(sendBuffer != NULL)
		free(sendBuffer);

	if(sendBuffer1 != NULL)
		free(sendBuffer1);

	if(recvBuffer != NULL)
		free(recvBuffer);
	sendBuffer = NULL;
	recvBuffer = NULL;
	uiCurSendLen = 0;
	uiCurRecvLen = 0;
	m_nWait = 0;
}

void CPTZControl::TimerHandle()
{
	CExAutoLock autoLock(&m_lock);
	m_bQuryZoomPos = TRUE;
	ResetEvent(m_hEventThread[1]);
	SetEvent(m_hEventThread[1]);
}

void CPTZControl::RecvByte(BYTE byRecv)
{
	if(uiCurRecvLen == 0){
		if(byRecv == 0xFF){
			if(m_tResponse == PELCO_RESPONSE_Null)
				return ;
			recvBuffer[uiCurRecvLen++] = byRecv;
			if(m_tResponse == PELCO_RESPONSE_Query)
				m_nWait = 18;
			else if(m_tResponse == PELCO_RESPONSE_Extended)
				m_nWait = 7;
			else
				m_nWait = 4;
		}
	}else{
		recvBuffer[uiCurRecvLen++] = byRecv;
		if(uiCurRecvLen == m_nWait){
			if(recvBuffer[3] != 0x59 && recvBuffer[3] != 0x5B 
				&& recvBuffer[3] != 0x5D && recvBuffer[3] != 0x63){
				TRACE("PTZ R> %d %02X %02X %02X %02X %02X %02X %02X\n", uiCurRecvLen,
					recvBuffer[0], recvBuffer[1], recvBuffer[2], recvBuffer[3], 
					recvBuffer[4], recvBuffer[5], recvBuffer[6]);
			}

			switch(recvBuffer[3])
			{
			case 0x59:
				m_iPanPos = recvBuffer[4];
				m_iPanPos <<= 8;
				m_iPanPos += recvBuffer[5];
				break;
			case 0x5B:
				m_iTiltPos = recvBuffer[4];
				m_iTiltPos <<= 8;
				m_iTiltPos += recvBuffer[5];
				break;
			case 0x5D:
				m_iZoomPos = recvBuffer[4];
				m_iZoomPos <<= 8;
				m_iZoomPos += recvBuffer[5];
				break;
			case 0x63:
				m_iMagnification = recvBuffer[4];
				m_iMagnification <<= 8;
				m_iMagnification += recvBuffer[5];
				break;
			}

			if(m_hEvent != NULL)
				SetEvent(m_hEvent);
			uiCurRecvLen = 0;
			m_nWait = 0;
		}
	}
}

int CPTZControl::SendCmd(LPPELCO_D_REQPKT pCmd, PELCO_RESPONSE_t tResp /* = PELCO_RESPONSE_General */)
{
	int iRet = 0;
	DWORD dwRet = WAIT_OBJECT_0;
	if(!m_bOpen)
		return -1;
	m_lock.Lock();
	m_tResponse = tResp;
	ResetEvent(m_hEvent);
	m_serial.Write((BYTE*)pCmd, sizeof(PELCO_D_REQPKT));
	if(tResp != PELCO_RESPONSE_Null)
		dwRet = WaitForSingleObject(m_hEvent, 500);
	/*else
		WaitForSingleObject(m_hEvent, 10);*/
	if( dwRet != WAIT_OBJECT_0 ){
		TRACE("PTZ send msg time out!\n");
		iRet = -1;
	}
	m_lock.Unlock();
	return iRet;
}

//////////////////////////////////////////////////////////////////////////
int CPTZControl::Move()
{
	int iRet = 0;
	CExAutoLock autoLock(&m_lock);
	
	if(m_iCurPanSpeed == m_iSetPanSpeed 
		&& m_iCurTiltSpeed == m_iSetTiltSpeed 
		&& m_iCurZoomSpeed == m_iSetZoomSpeed
		&&m_iCurIrisSpeed == m_iSetIrisSpeed){
		//TRACE("Speed no change !\n");
		return 0;
	}
	m_bChangeZoomSpeed = ((int)m_iCurZoomSpeed != (int)m_iSetZoomSpeed);
	m_bChangePanSpeed = ((int)m_iCurPanSpeed != (int)m_iSetPanSpeed);
	m_bChangeTiltSpeed = ((int)m_iCurTiltSpeed != (int)m_iSetTiltSpeed);
	m_bChangeIrisSpeed =((int)m_iCurIrisSpeed != (int)m_iSetIrisSpeed) ;

	m_iCurPanSpeed = m_iSetPanSpeed;
	m_iCurTiltSpeed = m_iSetTiltSpeed;
	m_iCurZoomSpeed = m_iSetZoomSpeed;
	m_iCurIrisSpeed = m_iSetIrisSpeed;

	if(!m_bTimerRun){
		ResetEvent(m_hEventThread[1]);
		SetEvent(m_hEventThread[1]);
	}
	return 0;
}

int CPTZControl::circle()
{

	Preset(PTZ_PRESET_GOTO, m_circle);
	m_circle = (m_circle+1)%5;

	return m_circle;
}

int CPTZControl::Preset(int nCtrlType, int iPresetNum)
{
	IPelcoDFormat::MakePresetCtrl(m_pReq, nCtrlType, iPresetNum, m_byAddr);
	return SendCmd(m_pReq, PELCO_RESPONSE_General);
}

int CPTZControl::Pattern(int nCtrlType, int iPatternNum)
{
	IPelcoDFormat::MakePatternCtrl(m_pReq, nCtrlType, iPatternNum, m_byAddr);
	TRACE("Pattern type = %d\n", nCtrlType);
	return SendCmd(m_pReq, PELCO_RESPONSE_General);
}

int CPTZControl::Query(int iQueryType)
{
	IPelcoDFormat::MakeExtCommand(m_pReq, iQueryType, 0, 0, m_byAddr);
	return SendCmd(m_pReq, PELCO_RESPONSE_Extended);
}

int CPTZControl::Dummy()
{
	IPelcoDFormat::MakeDummy(m_pReq, m_byAddr);
	return SendCmd(m_pReq, PELCO_RESPONSE_Null);
}

void CPTZControl::PtzThread()
{
	DWORD dwRet;
	while(TRUE){
		dwRet = WaitForMultipleObjects(2, m_hEventThread, NULL, INFINITE);
		if( dwRet == WAIT_OBJECT_0 ){
			break;
		}else if( dwRet == WAIT_OBJECT_0 + 1 ){
			MoveSync();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
int CPTZControl::MoveSync()
{
	int iRet = 0;
	CExAutoLock autoLock(&m_lock);

	if(m_bQuryZoomPos){
		IPelcoDFormat::MakeExtCommand(m_pReq, PTZ_QUERY_Zoom, 0, 0, m_byAddr);
		//iRet = SendCmd(m_pReq, PELCO_RESPONSE_Extended);
		//if(iRet == 0)   //zoom ������Ϣ�����������Ƿ�֧�֣�
		{
			m_pQTimer->KillQTimer();
		}
		m_bQuryZoomPos = FALSE;
		return iRet;
	}

	if(m_bChangeZoomSpeed || m_bChangeTiltSpeed || m_bChangePanSpeed||m_bChangeIrisSpeed){
		int Pan  = abs((int)(m_iCurPanSpeed));
		int Tilt = abs((int)m_iCurTiltSpeed);
		int Zoom = abs((int)m_iCurZoomSpeed);
		int Iris = abs((int)m_iCurIrisSpeed);

		//TRACE("PTZ: pan=%d tilt=%d\n", Pan, Tilt);

		m_cmd2Code = 0;
		if(m_iCurPanSpeed>0)
			m_cmd2Code |= 0x02;
		else if(m_iCurPanSpeed<0)
			m_cmd2Code |= 0x04;

		if(m_iCurTiltSpeed>0)
			m_cmd2Code |= 0x10;
		else if(m_iCurTiltSpeed<0)
			m_cmd2Code |= 0x08;
		
		if(m_iCurZoomSpeed > 0){		//Tele
			m_cmd2Code |= 0x20;
			m_bStopZoom = TRUE;
			//TRACE("TELE!\n");
		}else if(m_iCurZoomSpeed <0){//Wide
			m_cmd2Code |= 0x40;
			m_bStopZoom = TRUE;
			//TRACE("WIDE!\n");
		}else{
			if(m_bStopZoom){
				m_pQTimer->SetQTimer(50, (fpWAITORTIMERCALLBACK)TimerHandle_, this);
				TRACE("STOP!\n");
			}
			m_bStopZoom = FALSE;
		}

		m_data1Code = min(Pan, 63);
		m_data2Code = min(Tilt, 63);
		//m_data1Code = (m_data1Code < 5) ? 0 : m_data1Code;
		//m_data2Code = (m_data2Code < 5) ? 0 : m_data2Code;

		//TRACE("Pan %d Tilt %d Zoom %d\n", m_data1Code, m_data2Code, Zoom);

		m_cmd1Code=0;
		if(m_bChangeIrisSpeed)
		{
			if(m_iCurIrisSpeed > 0){
				m_cmd1Code |= 0x02;
			}else if(m_iCurIrisSpeed <0){
				m_cmd1Code |= 0x04;
			}
		}
		
//  		if(m_bChangeZoomSpeed){
//  			if(Zoom != 0)
//  				Zoom--;
//  			IPelcoDFormat::MakeExtCommand(m_pReq, 0x25, 0/*Zoom*/, 0, m_byAddr);
//  			SendCmd(m_pReq, PELCO_RESPONSE_General);
//  		}
		IPelcoDFormat::PktFormat(m_pReqMove, m_cmd1Code, m_cmd2Code, m_data1Code, m_data2Code, m_byAddr);
		iRet = SendCmd(m_pReqMove, PELCO_RESPONSE_Null);
		//TRACE("MoveSync:0x%x\n",m_cmd2Code);

		/*if(m_bChangeZoomSpeed){
			if(m_iCurZoomSpeed == 0){
				m_pQTimer->KillQTimer();
				m_bTimerRun = FALSE;
			}else if(!m_bTimerRun){
				m_pQTimer->SetQTimer(50, (fpWAITORTIMERCALLBACK)TimerHandle_, this);
				m_bTimerRun = TRUE;
			}
		}*/
		m_bChangePanSpeed = m_bChangeTiltSpeed = m_bChangeZoomSpeed = m_bChangeIrisSpeed = FALSE;
	}
	return iRet;
}


void CPTZControl::Menu()
{
	IPelcoDFormat::PktFormat(m_pReqMove, 0x00, 0x03, 0x00, 0x5F, 0x01);
	SendCmd(m_pReq, PELCO_RESPONSE_Null);
}
void CPTZControl::MenuEnter()
{
	IPelcoDFormat::PktFormat(m_pReqMove, 0x02, 0x00, 0x00, 0x00, 0x01);
	SendCmd(m_pReq, PELCO_RESPONSE_Null);
}
void CPTZControl::MenuUp()
{
	IPelcoDFormat::PktFormat(m_pReqMove, 0x00, 0x08, 0x00, 0x00, 0x01);
	SendCmd(m_pReq, PELCO_RESPONSE_Null);
	IPelcoDFormat::PktFormat(m_pReqMove, 0x00, 0x04, 0x00, 0x00, 0x01);
	SendCmd(m_pReq, PELCO_RESPONSE_Null);
}
void CPTZControl::MenuDown()
{
	IPelcoDFormat::PktFormat(m_pReqMove, 0x00, 0x10, 0x00, 0x00, 0x01);
	SendCmd(m_pReq, PELCO_RESPONSE_Null);
	IPelcoDFormat::PktFormat(m_pReqMove, 0x00, 0x04, 0x00, 0x00, 0x01);
	SendCmd(m_pReq, PELCO_RESPONSE_Null);
}
#endif


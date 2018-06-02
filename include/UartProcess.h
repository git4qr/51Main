#ifndef _UARTPROCESS_H_
#define _UARTPROCESS_H_
#include <stdint.h>
#include <sys/types.h>
#include "osa_thr.h"
#include "UserProcess.h"
#define     RECV_FRAME_LEN     (32)
#define     RECV_BUF_SIZE   (64)

typedef struct data_buf
{
	uint len;
	uint  pos;
	u_int8_t reading;
	u_int8_t  buf[RECV_BUF_SIZE];
}data_buf_t;

static data_buf_t swap_data = {0, 0, 0,{0}};

class CUartProcess : public CUserBase
{
public:
	CUartProcess();
	~CUartProcess();
	int open_Uart(const int nport);
	int set_Uart(const int baud_rate, const int data_bits,
	                           char parity, const int stop_bits);
	int UartFlush(int fd);
	int UartFlushWrite(int fd);
	int UartFlushRead(int fd);
	int Create();
	private:
	 	int  UartRecvDataThread();
 	 	int  UartSendDataThread();
 	 	int UartRecv(void *rcv_buf,int data_len);
 	 	int get_uartdata_process(unsigned char *uartdata, int size);
		OSA_ThrHndl        uartGetDataThrID;
		OSA_ThrHndl			uartSendDataThrID;
	int m_port;
	bool m_UartRun;
         void  prorcvbufdata(uint8_t *inBuf,int inCnt,uint8_t *outBuf,int *destCnt,int *readStatus);
protected:

	static 		void *uartGetDataThrFun(void *getContext)
	 		{
		     CUartProcess* sPthis =  (CUartProcess*)getContext;
	 			sPthis->UartRecvDataThread();

	 			return NULL;
	 		}

	static 		void *uartSendDataThrFun(void * sendContext)
	 		{
		   CUartProcess* sThis =  (CUartProcess*)sendContext;
	 			sThis->UartSendDataThread();

	 			return NULL;
	 		}


};

#endif

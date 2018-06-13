#ifndef _CCONNECT_H_
#define _CCONNECT_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include "UserProcess.h"
#include <stddef.h>
#include  "osa_thr.h"
#include  "osa_mutex.h"

typedef  int   CONNECT_IN;
typedef  struct  sockaddr_in  Sockaddress_IN;

class CConnect:public CUserBase {

public:
	 	 	 CConnect (CONNECT_IN mConnectd, Sockaddress_IN mSockAddress );
	 	 	 ~CConnect();

	 	 	 void startRunning();
	 	 	 bool  bConnecting;

	 		bool IsConnecting(void){
	 					return  bConnecting;
	 					}

	 		void		DisConnecting(void){
	 			     bConnecting = FALSE;
	 					}

private:
	 	    int writen(int fd, void *vptr, int  n);
	 	   int readn(int fd, void *vptr, int n);
	 	 	int  RecvDataThread();
	 	 	int  SendDataThread();
	 	 	int NETRecv(void *rcv_buf,int data_len);
	 	 	CONNECT_IN		   m_connect;;
	 		Sockaddress_IN	   m_addr;
	 		OSA_ThrHndl        getDataThrID;
	 		OSA_ThrHndl			sendDataThrID;

protected:

	static 		void *getDataThrFun(void * mContext)
	 		{
	 			CConnect* sPthis =  (CConnect*)mContext;
	 			sPthis->RecvDataThread();

	 			return NULL;
	 		}

	static 		void *sendDataThrFun(void * Context)
	 		{
	 			CConnect* sThis =  (CConnect*)Context;
	 			sThis->SendDataThread();

	 			return NULL;
	 		}

};

#endif

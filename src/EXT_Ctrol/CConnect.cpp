#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <error.h>
#include "CConnect.h"

#define     RECVSIZE  (32)

void printfRCVbuff(char *mBuf,int size);

CConnect::CConnect (CONNECT_IN mConnectd, Sockaddress_IN  mSockAddress )
{
	m_connect = mConnectd;
	m_addr= mSockAddress;
	bConnecting=false;
}

CConnect::~CConnect()
{
	bConnecting = false;
	OSA_thrJoin(&getDataThrID);
	OSA_thrJoin(&sendDataThrID);
}

void CConnect::startRunning()
{
			int iret;
			iret =   OSA_thrCreate(&getDataThrID,
												 getDataThrFun,
																0,
																0,
														(void*)this);

			iret =   OSA_thrCreate(&sendDataThrID,
													sendDataThrFun,
																0,
																0,
														(void*)this);
}

int  CConnect::RecvDataThread()
{
	int reVal;
	char  tmpRcvBuff[RECVSIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));
	while(bConnecting){
		    reVal = recv(m_connect,tmpRcvBuff,RECVSIZE,0);
		    if(errno ==EINTR){
		    	continue;
		    }
		    if(reVal<=0){
		    			printf ("INFO: Disconnect the link!!\r\n");
		    			break;
		    	}
		    	else{
		    			//process the rcvbuff data  and clean buff

		    		//test receive
		    		//printfRCVbuff(tmpRcvBuff,RECVSIZE);
		    	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));   //clean buff
		    }
	  }
	bConnecting = false;
	return 0;
}

char *test="asdfghjkl";

int  CConnect::SendDataThread()
{
	    int result,retVle;
	    struct timeval    sendTimeout;
		while(bConnecting){
			sendTimeout.tv_sec   = 3;
			sendTimeout.tv_usec = 33000;
			 result = select(0,NULL, NULL, NULL, &sendTimeout);
			if(result <0){
				printf("ERR: Connect --select Send error\r\n");
			            break;
			}
		  if(result == 0){
			  //test send
			    retVle = send (m_connect,test,sizeof(test),0);
			    if(retVle<=0){
			       break;
			    }else   continue;
		      }
		  }
		return 0;
}

void printfRCVbuff(char *mBuf,int size)
{
	for(int i=0;i<size;i++)
		printf("INFO: RCVbuf ID: %d---Value: %c \r\n ",i,mBuf[i]);
}


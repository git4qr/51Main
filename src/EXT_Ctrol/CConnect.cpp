#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <error.h>
#include "CConnect.h"
#include "osa_sem.h"

extern OSA_SemHndl m_semHndl;
#define     RECVSIZE  (32)

void printfRCVbuff(char *mBuf,int size);
static u_int8_t  frame_head[]={0xEB, 0x53};
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

int  CConnect::NETRecv(void *rcv_buf,int data_len)
{
		int fs_sel,len;
		fd_set fd_netRead;
		struct timeval timeout;
		FD_ZERO(&fd_netRead);
		FD_SET(m_connect,&fd_netRead);
	    timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		fs_sel = select(m_connect+1,&fd_netRead,NULL,NULL,&timeout);
		if(fs_sel){
			len = read(m_connect,rcv_buf,data_len);
			return len;
		}
		else if(-1 == fs_sel){
			printf("ERR: Uart Recv  select  Error!!\r\n");
			return -1;
		}
		else if(0 == fs_sel){
			//printf("Warning: Uart Recv  time  out!!\r\n");
			return 0;
		}
}

int  CConnect::RecvDataThread()
{
	int reVal;
	char tmpNetRcvBuff[RECVSIZE];
	memset(tmpNetRcvBuff,0,sizeof(tmpNetRcvBuff));
	while(bConnecting){
		   // reVal = recvfrom(m_connect,tmpNetRcvBuff,RECVSIZE,0,0,0);
		    reVal= NETRecv(tmpNetRcvBuff,RECVSIZE);
		    if(errno ==EINTR){
		    	continue;
		    }
		    if(reVal<=0){
		    			printf ("INFO: Disconnect the link!!\r\n");
		    			break;
		    	}
		    	else{
                           printf("INFO:Net process!, Net receive data :\r\n");
		    		      for(int i=0;i<reVal;i++)
		    		       {
		    				 //  printf("%02x ",tmpNetRcvBuff[i]);
		    				   rcvBufQue.push_back(tmpNetRcvBuff[i]);
		    				}
                          // printf("\r\n");
		    		       memset(tmpNetRcvBuff,0,sizeof(tmpNetRcvBuff));
		    			   if(rcvBufQue.size()<5)   continue;    //less the smallest  frame   and then  receive the data
		    	           int off_t=findFrameHeader();     //find  frame heade
		    	            if(off_t<0){
		    	            	 rcvBufQue.clear();
		    	            	 continue;
		    				}
		    	             if(off_t)
		    	            rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+off_t);  // erase  unused bytes
		    	            	//now vector start 0xEB  0x53
		    	           if(rcvBufQue.size()<5)    continue; //less the smallest  frame   and then continue  receive the data
		    	           if( rcvBufQue.at(2)>rcvBufQue.size())   continue;  // judge the  length  byte  upper the  vector byte size
		    	             prcRcvFrameBufQue();
		    }
	  }
	bConnecting = false;
	return 0;
}

#if 1
int  CConnect::SendDataThread()
{
	    int retVle,n;
		while(bConnecting){
			   OSA_semWait(&m_semHndl,OSA_TIMEOUT_FOREVER);
			memcpy(&repSendBuffer.sendBuff[0],frame_head,sizeof(frame_head));
			     getSendInfo(feedback, &repSendBuffer);
		     //   for(n=0;n<repSendBuffer.byteSizeSend;n++)
			            //  printf("%02x ",repSendBuffer.sendBuff[n]);
		            // printf("\n");
			        retVle = write (m_connect, &repSendBuffer.sendBuff,repSendBuffer.byteSizeSend);
			    }
		return 0;
}

#else if
// test
char *test="asdfghjkl";
int  CConnect::SendDataThread()
{
	    int result,retVle,n;
	    struct timeval    sendTimeout;
		while(bConnecting){
			  // OSA_semWait(&m_semHndl,OSA_TIMEOUT_FOREVER);
			   sendTimeout.tv_sec   = 2;
				sendTimeout.tv_usec = 50000;
				 result = select(0,NULL, NULL, NULL, &sendTimeout);
				if(result <0){
					printf("ERR: Connect --select Send error\r\n");
				            break;
				}
			  if(result == 0){
				 memcpy(&repSendBuffer.sendBuff[0],frame_head,sizeof(frame_head));
			     getSendInfo(3, &repSendBuffer);
		         for(n=0;n<repSendBuffer.byteSizeSend;n++)
			              printf("%02x ",repSendBuffer.sendBuff[n]);
		             printf("\n");
			        retVle = write (m_connect, &repSendBuffer.sendBuff,repSendBuffer.byteSizeSend);
			    }
		  }
		return 0;
}
#endif
void printfRCVbuff(char *mBuf,int size)
{
	for(int i=0;i<size;i++)
		printf("INFO: RCVbuf ID: %d---Value: %c \r\n ",i,mBuf[i]);
}


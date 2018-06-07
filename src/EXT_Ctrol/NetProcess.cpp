#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "NetProcess.h"
#include "CConnect.h"

typedef  vector<CConnect*>  CConnectVECTOR;
 CConnectVECTOR  connetVector;

CNetWork::CNetWork()
{
		m_Socketfd=0;
		exitservThrd=true;
		clientConnect =false;
		m_connectfd=0;
		memset(&servAddr,0,sizeof(servAddr));
		memset(&connectAddr,0,sizeof(connectAddr));
		memset(&netAcceptThrd,0,sizeof(netAcceptThrd));
		memset(&netCleanThrd,0,sizeof(netCleanThrd));
		memset(&mutexConn,0,sizeof(mutexConn));
}
CNetWork::~CNetWork()
{

}

int CNetWork::NetOpen()
{
	m_Socketfd= socket(AF_INET,SOCK_STREAM,0);
	if(m_Socketfd<0){
		printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
		}
	return 0;
}

int CNetWork::NetBind()
{
	  int iRet;
      char ipbuf[20];
      char *ipaddr=ipbuf;
      struct ifreq ifr;
      servAddr.sin_family = AF_INET;
      servAddr.sin_port = htons( _LisPort);
      strcpy(ifr.ifr_name, "eth0");
      //SIOCGIFADDR标志代表获取接口地址
     if (ioctl(m_Socketfd, SIOCGIFADDR, &ifr) <  0)  perror("net ioctl Error");
      ipaddr=inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
      servAddr.sin_addr.s_addr= inet_addr(ipaddr);//htonl(INADDR_ANY);
      printf("INFO: server  addr:%d---port: %d\r\n",servAddr.sin_addr.s_addr,servAddr.sin_port);
       iRet = bind(m_Socketfd,(struct sockaddr*)&servAddr,sizeof(servAddr));
       if (iRet<0){
            printf("ERR:  Can not bind  Socketfd, error: %s(errno: %d)\r\n ",strerror(errno),errno);
       }

      return  0 ;
}

int  CNetWork::NetListen()
{
      int ret;
       ret =listen(m_Socketfd,MAXCONNECT);
       if(ret<0){
    	  printf("ERR: Can not listen sockfd ,error: %s(errno: %d)\r\n",strerror(errno),errno);
       }
       exitservThrd=false;
     return 0;
}


int  CNetWork::Create()
{
	int retv;
	OSA_mutexCreate(&mutexConn);
	  NetOpen();
	  NetBind();
	  NetListen();
     retv=OSA_thrCreate(&netAcceptThrd,
    		 	 	 	 	 	 	 	 NetAcceptThrdFunc,
    		 	 	 	 	 	 	 	 	 0,
    		 	 	 	 	 	 	 	 	 0,
    		 	 	 	 	 	 	 	 	 (void*)this);

     retv=OSA_thrCreate(&netCleanThrd,
    		 	 	 	 	 	 	 	 NetCleanThrdFunc,
    		 	 	 	 	 	 	 	 	 0,
    		 	 	 	 	 	 	 	 	 0,
    		 	 	 	 	 	 	 	 	 (void*)this);
	 return 0;
}

int CNetWork::AccpetLinkThread()
{
        int lenthConnect = sizeof(connectAddr);
        while(!exitservThrd){
    	m_connectfd=accept(m_Socketfd, (struct sockaddr*)&connectAddr,(socklen_t*)&lenthConnect);
            			if(m_connectfd<0){
            					printf("ERR: Can not  accept  Client connect\r\n");
            					continue;
            			}
            			else{
            	                    clientConnect=true;;
                                    CConnect  *pConnect = new CConnect (m_connectfd, connectAddr);
                                    OSA_mutexLock(&mutexConn);
                                    char *connectIP = inet_ntoa (connectAddr.sin_addr);
                                    int connectPort = ntohs (connectAddr.sin_port);
                                    printf("INFO: Accept Connect IP : %s---- Port: %d\r\n",connectIP, connectPort);
                                    connetVector.push_back(pConnect);
                                    pConnect->bConnecting=true;
                                    OSA_mutexUnlock(&mutexConn);
                                    pConnect->startRunning();
            			}
        }
        return 0;
}

int CNetWork::ReclaimConnectThread()
{
	  //客户端退出
        while(!exitservThrd){
        	 OSA_mutexLock(&mutexConn);
        	 CConnectVECTOR::iterator iter = connetVector.begin();
        			for (iter; iter != connetVector.end();)
        			{
        				CConnect *pCLink= (CConnect*)*iter;
        				if (!pCLink->IsConnecting())
        				{
        					printf("INFO:  Reclainm connect !!\r\n");
        					connetVector.erase(iter);
        					delete pCLink;
        					pCLink = NULL;
        				}else{
        					iter++;						//指针下移
        				}
        			}
        			if(connetVector.size() == 0) {
        				clientConnect = false;;
        	        }
        			OSA_mutexUnlock(&mutexConn);
        			sleep(TIME_FOR_THREAD_END);
        }
      //服务器端退出
        if(exitservThrd){
        	   OSA_mutexLock(&mutexConn);
        	   CConnectVECTOR::iterator iter = connetVector.begin();
        		for (iter; iter != connetVector.end();)
        		{
        			CConnect *pClient = (CConnect*)*iter;
        			//如果客户端的连接还存在，则断开连接，线程退出
        			if (pClient->IsConnecting())
        			{
        				pClient->DisConnecting();
        			}
        			++iter;
        		}
        		   OSA_mutexUnlock(&mutexConn);
        		sleep(TIME_FOR_THREAD_END);
         }
        return 0;
}

int CNetWork::Destrory()
{
	exitservThrd = true;
	OSA_thrJoin(&netAcceptThrd);
	OSA_thrJoin(&netCleanThrd);
	return 0;
}

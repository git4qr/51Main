#ifndef _NETPROCESS_H_
#define _NETPROCESS_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include "osa_thr.h"
#include  "osa_mutex.h"

#define  _LisPort  (10000)
#define  MAXCONNECT   (10)
#define  TIME_FOR_THREAD_END      (3)


class CNetWork{

public:
	  CNetWork();
	~CNetWork();
    int Create();
    int Destrory();

private:
     int NetOpen();
     int NetBind();
     int NetListen();
     int AccpetLinkThread();
     int ReclaimConnectThread();
      static void  *NetAcceptThrdFunc(void* AcceptContext)
      {
               CNetWork  *m_thrd =  (CNetWork *)AcceptContext;
               m_thrd->AccpetLinkThread();
               return NULL;
      }

      static void  *NetCleanThrdFunc(void* CleanContext)
      {
               CNetWork  *pThrd =  (CNetWork *)CleanContext;
               pThrd->ReclaimConnectThread();
               return NULL;
      }

    struct sockaddr_in   servAddr,  connectAddr;
     int m_Socketfd;
     int m_connectfd;
     OSA_ThrHndl   netAcceptThrd,netCleanThrd;
     OSA_MutexHndl    mutexConn;
     bool exitservThrd;
     bool  clientConnect;

protected:



};
#endif

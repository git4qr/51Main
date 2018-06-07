
#ifndef _IMESSAGEQUE_H_
#define _IMESSAGEQUE_H_
#pragma once

#ifdef    USE_HANDNL

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define   MAXQUESIZE  (128)

#define MES_SOK      0  ///< Status : OK
#define MES_EFAIL   -1  ///< Status : Generic error

#define MES_TIMEOUT_NONE        ((Uint32) 0)  // no timeout
#define MES_TIMEOUT_FOREVER     ((Uint32)-1)  // wait forever
#define MES_memAlloc(size)      (void*)malloc((size))
#define MES_memFree(ptr)        free(ptr)

#define MES_ERROR(...) \
  do \
  { \
  fprintf(stderr, " ERROR  (%s|%s|%d): ", __FILE__, __func__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
  } \
  while(0);
  
typedef enum {	
	type_jos=1,
	type_net,
	type_local,
	type_ipc,
	type_com
}MesType;

typedef struct  MsgQue{
		int lenth;
		MesType ; 
		void  *pdate;
}QueMessage;


typedef struct {

  Uint32 curRd;
  Uint32 curWr;
  Uint32 len;
  Uint32 count;
  void *queue;
  pthread_mutex_t lock;
  pthread_cond_t  condRd;
  pthread_cond_t  condWr;
  
}Work_QueHndl;

int Work_queCreate(Work_QueHndl *hndl);
int Work_queDelete(Work_QueHndl *hndl);
int Work_quePut(Work_QueHndl *hndl, MesType  mtype , Uint32 timeout);
int Work_queGet(Work_QueHndl *hndl, Int32 *value, Uint32 timeout);
int Work_quePeek(Work_QueHndl *hndl, Int32 *value);
Uint32 Work_queGetQueuedCount(Work_QueHndl *hndl);
Bool Work_queIsEmpty(Work_QueHndl *hndl);
typedef    Work_QueHndl    CMESGQUEHndl;
CMESGQUEHndl  procQue;
#else

#include "osa_que.h"
#include "app_status.h"
#define  MAXQUESIZE  (128)
#define MES_SOK      0  ///< Status : OK
#define MES_EFAIL   -1  ///< Status : Generic error
#define MES_TIMEOUT_NONE        ((Uint32) 0)  // no timeout
#define MES_TIMEOUT_FOREVER     ((Uint32)-1)  // wait forever
#define MES_memAlloc(size)      (void*)malloc((size))
#define MES_memFree(ptr)        free(ptr)

typedef    OSA_QueHndl*    CMESGQUEHndl;
typedef    OSA_QueHndl   CMESGQUE;
int GetMesQueMem();
int Work_queCreate();
int Work_queDelete();
int Work_quePut( Int32  value );
int Work_queGet( Int32 *value );
int Work_quePeek( Int32 *value);
Uint32 Work_queGetQueuedCount();
Bool Work_queIsEmpty();


#pragma pack (4)
typedef struct{
      unsigned char SecAcqStat;// eSecTrkMode
     int ImgPixelX; //SecTrk X
     int ImgPixelY; //SecTrk  Y
}selectTrack;

typedef struct{
	volatile unsigned char AvtMoveX;// eTrkRefine (axis or aim) x
	volatile unsigned char AvtMoveY;// eTrkRefine (axis or aim) y
}POSMOVE;

typedef struct{
	volatile int AcqBoxW[eSen_Max];
	volatile int AcqBoxH[eSen_Max];
}AcqBoxSize;
#pragma pack()

#define MES_ERROR(...) \
  do \
  { \
  fprintf(stderr, " ERROR  (%s|%s|%d): ", __FILE__, __func__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
  } \
  while(0);

#endif
#endif


#include "iMessageQUE.h"

 static CMESGQUE  WorkQue;
 static CMESGQUEHndl  procQue=&WorkQue;
 //extern OSA_SemHndl  queReady;
int  Work_queCreate()
{
	  pthread_mutexattr_t mutex_attr;
	  pthread_condattr_t cond_attr;
	  int status=MES_SOK;
	  procQue->curRd = procQue->curWr = 0;
	  procQue->count = 0;
	  procQue->len   = MAXQUESIZE;
	  procQue->queue = (Int32 *)MES_memAlloc(sizeof(Int32)*procQue->len);

	  if(procQue->queue==NULL) {
	    OSA_ERROR("OSA_queCreate() = %d \r\n", status);
	    return MES_EFAIL;
	  }
	  status |= pthread_mutexattr_init(&mutex_attr);
	  status |= pthread_condattr_init(&cond_attr);
	  status |= pthread_mutex_init(&procQue->lock, &mutex_attr);
	  status |= pthread_cond_init(&procQue->condRd, &cond_attr);
	  status |= pthread_cond_init(&procQue->condWr, &cond_attr);

	  if(status!=OSA_SOK)
	    MES_ERROR("Work_queCreate() = %d \r\n", status);

	  pthread_condattr_destroy(&cond_attr);
	  pthread_mutexattr_destroy(&mutex_attr);

	  return status;
}


int Work_queDelete()
{
  if(procQue->queue!=NULL)
    MES_memFree(procQue->queue);
    
  pthread_cond_destroy(&procQue->condRd);
  pthread_cond_destroy(&procQue->condWr);
  pthread_mutex_destroy(&procQue->lock);  
  
  return MES_SOK;
}

int Work_quePut(Int32  value)
{
	int status = MES_EFAIL;

   pthread_mutex_lock(&procQue->lock);

  while(1) {
    if( procQue->count < procQue->len ) {
      procQue->queue[procQue->curWr] = value;
      procQue->curWr = (procQue->curWr+1)%procQue->len;
      procQue->count++;
      status = MES_SOK;
      pthread_cond_signal(&procQue->condRd);
    //  OSA_semSignal(&queReady);
      break;
    } else {
     // if(timeout == MES_TIMEOUT_NONE)
       // break;
      status = pthread_cond_wait(&procQue->condWr, &procQue->lock);
    }

  }

  pthread_mutex_unlock(&procQue->lock);
  return status;
  
}

int Work_queGet(Int32 *value)
{
  int status = MES_EFAIL;
  pthread_mutex_lock(&procQue->lock);
  while(1) {
    if(procQue->count > 0 ) {
      if(value!=NULL) {
        *value = procQue->queue[procQue->curRd];
      }
      procQue->curRd = (procQue->curRd+1)%procQue->len;
      procQue->count--;
      status = MES_SOK;
      pthread_cond_signal(&procQue->condWr);
      break;
    } else {
     // if(timeout == OSA_TIMEOUT_NONE)
     //   break;

      status = pthread_cond_wait(&procQue->condRd, &procQue->lock);
    }
  }

  pthread_mutex_unlock(&procQue->lock);

  return status;

}

Uint32 Work_queGetQueuedCount()
{
  Uint32 queuedCount = 0;

  pthread_mutex_lock(&procQue->lock);
  queuedCount = procQue->count;
  pthread_mutex_unlock(&procQue->lock);
  return queuedCount;
}

int Work_quePeek( Int32 *value)
{
  int status = MES_EFAIL;
  pthread_mutex_lock(&procQue->lock);
  if(procQue->count > 0 ) {
      if(value!=NULL) {
        *value = procQue->queue[procQue->curRd];
        status = MES_SOK;
      }
  }
  pthread_mutex_unlock(&procQue->lock);

  return status;
}

Bool Work_queIsEmpty()
{
  Bool isEmpty;

  pthread_mutex_lock(&procQue->lock);
  if (procQue->count == 0)
  {
      isEmpty = TRUE;
  }
  else
  {
      isEmpty = FALSE;
  }
  pthread_mutex_unlock(&procQue->lock);

  return isEmpty;
}



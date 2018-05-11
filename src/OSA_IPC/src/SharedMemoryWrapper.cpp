#include "SharedMemoryWrapper.h"
#include <stdio.h>
 #include <string.h>

key_t sharedKeyGet(char *path,int proj_id)
{

 key_t key = ftok(path,proj_id);
    if(key < 0){
        printf("%d:%s\n",errno,strerror(errno));
        return -1;
    }
     return key;
}
int sharedMemoryGet(key_t key, int size)
{
	int shmid;
	
	if ((shmid = shmget(key, size, 0666)) == -1)
		return -1;

	return shmid;
}

int sharedMemoryCreate(key_t key, int size)
{
	int shmid;

	if ((shmid = shmget(key, size, IPC_CREAT | 0666)) == -1)
		return -1;

	return shmid;
}

int sharedMemoryCreateOrGet(key_t key, int size)
{
	int shmid;

	/* If memory has already been created.. then just get it: */
	if ((shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666)) == -1)
	{
		if( errno != EEXIST )
			return -1;

		if ((shmid = shmget(key, size, 0666)) == -1)
			return -1;
	}

	return shmid;
}

int sharedMemoryCreateIfGone(key_t key, int size)
{
	int shmid;

	/* If memory has already been created.. then just get it: */
	if ((shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666)) == -1)
		return errno;

	return shmid;
}

void* sharedMemoryAttach(int shmid)
{
	return shmat(shmid, 0, 0);
}

int sharedMemoryDetatch(const void* shmaddr)
{
	if( shmdt(shmaddr) == -1 )
		return errno;

	return 0;
}

int sharedMemoryDelete(int shmid)
{
	if( shmctl(shmid, IPC_RMID, 0) == -1 )
		return errno;

	return 0;
}

int sharedMemoryLock(int shmid)
{
	if( shmctl(shmid, SHM_LOCK, 0) == -1 )
		return errno;

	return 0;
}

int sharedMemoryUnlock(int shmid)
{
	if( shmctl(shmid, SHM_UNLOCK, 0) == -1 )
		return errno;

	return 0;
}

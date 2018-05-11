#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <iostream>
#include "osa_thr.h"
#include "iMessageQUE.h"
#include "msg_id.h"
#include "app_status.h"
#include "UserProcess.h"


//static bool JosStart = true;

#define joystick_Dev 	"/dev/input/js0"
#define JS_EVENT_BUTTON 0x01 
#define JS_EVENT_AXIS 0x02 
#define JS_EVENT_INIT 0x80 



typedef int	INT32;
typedef short	INT16;
typedef char	INT8;
typedef unsigned int	UINT32;
typedef unsigned short	UINT16;
typedef unsigned char	UINT8;
typedef long		LONG;
typedef unsigned long	ULONG;

typedef struct js_event {
    UINT32 time;   
    INT16 value; 
    UINT8 type; 
    UINT8 number; 
}joy_event;

//static bool JosStart = true;

class CJosStick : public CUserBase
{
public:
	CJosStick();
~CJosStick();
void Create();
void Init();
void Config();
void Run();
void Stop();
void Destroy();
void JoystickProcess();

int open_joystick(char *joystick_device);
int read_joystick_event(joy_event *jse);


public:
	OSA_ThrHndl m_thrJoy;
	static void *josEventFunc(void *context)
	{
		CJosStick *User = (CJosStick*)context;
		while(JosStart){
		User->JoystickProcess();
		}
	}
private:
	joy_event *jse;
	static bool JosStart;
};

#endif

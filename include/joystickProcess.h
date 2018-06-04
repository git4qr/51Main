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
int Create();
int  Config();
int  Run();
int  Stop();
int  Destroy();
void JoystickProcess();
int JosToWinX(int x);
int JosToWinY(int y);
public:
	OSA_ThrHndl m_thrJoy;
	static void *josEventFunc(void *context)
	{
		CJosStick *User = (CJosStick*)context;
		while(JosStart){
		User->JoystickProcess();
		}
		return  NULL;
	}
private:
	int open_joystick(char *joystick_device);
	int read_joystick_event(joy_event *jse);
	void procJosEvent_Axis(UINT8  mjosNum );
    void ProcJosEvent_Button(UINT8 njosNum);
   void  ProcEvent_Button(int  EventBtnNum, int EventValue);
    void Button_Event(u_int8_t  btnNum);
    void EventAixs(u_int8_t  axisNum);
	joy_event *jse;
	static bool JosStart;

	bool isTrackIn;
};

#endif

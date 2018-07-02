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

typedef struct josBtn{
     int jos_btn_1; 
     int jos_btn_2;
     int jos_btn_3; 
     int jos_btn_4; 
     int jos_btn_5; 
     int jos_btn_6; 
     int jos_btn_7; 
     int jos_btn_8; 
     int jos_btn_9; 
     int jos_btn_10; 
     int jos_btn_11;
     int jos_btn_12;
}josBtn_t;

typedef struct josAxis{
     int jos_axis_1_x;
     int jos_axis_1_y; 
     int jos_axis_2_x;//pov
     int jos_axis_2_y; 
     int jos_axis_3_x; 
     int jos_axis_3_y;
}josAxis_t;

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
	void updateJosKeyMap();
	void updateJosAxisMap();

	josBtn_t m_josBtn_t;
	josAxis_t m_josAxis_t;
	int josKeyMap[MSGID_INPUT_Max];
	int JosAxisMap[6];

private:
	int open_joystick(char *joystick_device);
	int read_joystick_event(joy_event *jse);
	void procJosEvent_Axis(UINT8  mjosNum );
    void ProcJosEvent_Button(UINT8 njosNum);
   void  ProcEvent_Button(int  EventBtnNum, int EventValue);
    void Button_Event(u_int8_t  btnNum);
    void EventAixs(u_int8_t  axisNum);
    void Y_CtrlIrisAndFocus(int value);
    void X_CtrlMmtSelect(int value);
	joy_event *jse;
	static bool JosStart;
	bool isTrackIn;
};

#endif

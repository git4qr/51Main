#ifndef __CMESGLINK_H__
#define __CMESGLINK_H__

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

using namespace std;

class CUserBase
{
public:
	CUserBase();
	~CUserBase();

public:
	int *JOS_Value;
	virtual void Create(){};
	virtual void Init(){};
	virtual void Config(){};
	virtual void Run(){};
	virtual void Stop(){};

	virtual void Destroy(){
		delete JOS_Value;
	};

protected:
	volatile unsigned int  SensorStat;
	volatile unsigned int  AvtTrkAimSize;

	virtual void EnableTrk(bool bEnable)
	{
		SendMsg(Cmd_Mesg_TrkCtrl, bEnable);
	};
	virtual void SelSensor()
	{
		SendMsg(Cmd_Mesg_SensorCtrl, 0);
	};
	virtual void TrkBoxCtrl()
	{
		SendMsg(Cmd_Mesg_TrkBoxCtrl, 0);
	};
	virtual void EnableTrkSearch(bool bEnable)
	{
		SendMsg(Cmd_Mesg_TrkSearch, bEnable);
	};
	virtual void IrisUp(bool bEnable)
	{
		SendMsg(Cmd_Mesg_IrisUp, bEnable);
	};
	virtual void IrisDown(bool bEnable)
	{
		SendMsg(Cmd_Mesg_IrisDown, bEnable);
	};
	virtual void FocusUp(bool bEnable)
	{
		SendMsg(Cmd_Mesg_FocusFar, bEnable);
	};
	virtual void FocusDown(bool bEnable)
	{
		SendMsg(Cmd_Mesg_FocusNear, bEnable);
	};
	virtual void EnableIMG(bool bEnable)
	{
		SendMsg(Cmd_Mesg_ImgEnh, bEnable);
	};
	virtual void EnableAuto_Iris_Focus(bool bEnable)  //预留
	{
		SendMsg(Cmd_Mesg_unable, bEnable);
	};
	virtual void AIMPOS_X()
	{
		SendMsg(Cmd_Mesg_AIMPOS_X, 0);
	};
	virtual void AIMPOS_Y()
	{
		SendMsg(Cmd_Mesg_AIMPOS_Y, 0);
	};
	virtual void ZoomLongCtrl()
	{
		SendMsg(Cmd_Mesg_ZoomLong, 0);
	};
	virtual void ZoomShortCtrl()
	{
		SendMsg(Cmd_Mesg_ZoomShort, 0);
	};
	virtual void AXIS_X()
	{
		SendMsg(Cmd_Mesg_AXISX, 0);
	};
	virtual void AXIS_Y()
	{
		SendMsg(Cmd_Mesg_AXISY, 0);
	};

private:
	int SendMsg(int MsgId, int Option)
	{
		Work_quePut(MsgId);
		return 0;
	}
};


#endif

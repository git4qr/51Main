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
#include <map>
#include <vector>
#include "osa_thr.h"
#include "iMessageQUE.h"
#include "msg_id.h"
#include "app_status.h"
#include "Ipcctl.h"
using namespace std;

#define   RCVBUFFSIZELENTH  (2048)


typedef struct
{
	//char 0
	u_int8_t  InfoHeader;
	//char 1
	u_int8_t	InfoId;
	//char 2   控制命令control cmd
	uint 	checkAxisSave :1;
	uint	    checkAxisEn :1;
	uint	     startAutoChk :1;
	uint	     trackEn :1;
	uint      TVImgEnh :1;
	uint   	 FRImgEnh :1;
	uint      VideoSwitch:2;
	//char3  跟踪微调/校轴信息
	uint      crossLineTiltMov:2;
	uint      crossLinePanMov:2;
	uint      trackTiltFinetune:2;
	uint      trackPanFinetune:2;
	//char 4   多目标选择编号/波门大小选择
	uint   reserve0:1;
    uint   trkBoxSizeCtl:2;
    uint    mutilTargetDect:1;
    uint    selectmutilTargeNum:4;
	//char 5  缩放及画中画
    uint    picpEn:1;
    uint    picpSource:1;
    uint    picpZoomEn:1;
    uint    picpZoomOpt:2;
    uint    mFrameZoomEn:1;
    uint    mFrameZoomopt:2;
    //char 6  跟踪搜索功能高位信息
    uint   selTrkHighByteHon:4;
    uint   selTrkHighByteVel:4;
    //char 7  跟踪搜索功能低位信息（水平方向）
    u_int8_t   selTrkLowByteHon;
    //char 8  跟踪搜索功能低位信息（垂直方向）
    u_int8_t   selTrkLowByteVel;
    //char 9   工作模式信息
    uint currtMode:4;
    uint  reserve1:4;
    //char 10 字符显示控制
    uint characterColor:4;
    uint  reserve2:4;
    //char 11 传感器上电状态显示
    uint TVOnStat:1;
    uint FROnStat:1;
    uint laserOnStat:1;
    uint  reserve3:5;
    //char 12-13  电视传感器视场角
    ushort   TVFOVAngle;
    //char 14-15  热像传感器视场角
    ushort   FRFOVAngle;
    //char 16 激光器信息
    uint laserCoding:4;
    uint  reserve4:4;
    //char 17 -18目标距离
   ushort  targetDistance;
   //char 19-20 转塔方位角度
   short turretPanAngle;
   //char 21-22 转塔俯仰角度
   short turretTiltAngle;
   //char 23 视频传输与帧率设置
   uint FRframeFreq:2;
   uint TVframeFreq:2;
   uint videoTransMode:2;
   uint reserve5:2;
  //char 24视频压缩指令
   uint TVCompsquality:2;
   uint FRCompsquality:2;
   uint reserve6:4;
 //char 25 跟踪算法/检测模式/增强模式
   uint  enHanceMode:3;
   uint dectectMode:2;
   uint trackarithmetic:3;
   //char 26 -27平台控制x
   ushort  plantCtrlX;
   //char 28-29平台控制y
   ushort  plantCtrlY;
   //char 相机控制/跟踪搜索
   uint searchTrack:2;
   uint focusSet:2;
   uint IrisSet:2;
   uint opticalZoom:2;
	//char 31
	u_int8_t     	chksum;
}EXT_CtrlInput;

typedef   void (*pExtEventFunc)(void);
typedef    map<int,pExtEventFunc>  MAP_KYENUM_TO_PFUNC;
typedef    map<int,int>  MAP_KYENUM_TO_PUTBUFFID;

typedef struct {
	int   eventValue;
	pExtEventFunc  extProcFuc;
}ExtEvent;

typedef  struct  {
	u_int8_t   cmdBlock;
	u_int8_t   cmdFiled;
	int    confitData;
}systemSetting;

class CUserBase
{
public:
	CUserBase();
	~CUserBase();
	int *ExtInputCtrl;
	vector<int> ExtInputCtrlValue;
	virtual int  Create(){return 0;};
	virtual int Init(){return 0;};
	virtual int  Config(){return 0;};
	virtual void PreInit();
	virtual int Run(){return 0;};
	virtual int  Stop(){return 0;};
	int process_ExtInputData( u_int8_t *data_buf, uint data_len);
	int *EXT_Ctrl;
	IMGSTATUS  avt_status;
	int errorOutPut[2];
	systemSetting avtSetting;

	virtual int  Destroy(){
		delete EXT_Ctrl;;
		return 0;
	};
#if 0
	virtual void EnableTrk()
		{
			SendMsg(Cmd_Mesg_TrkCtrl);
		};
	 virtual  void SelSensor()
		{
			SendMsg(Cmd_Mesg_SensorCtrl);
		};
		virtual void TrkBoxCtrl()
		{
			SendMsg(Cmd_Mesg_TrkBoxCtrl);
		};
		virtual void EnableTrkSearch()
		{
			SendMsg(Cmd_Mesg_TrkSearch);
		};
		virtual void IrisUp()
		{
			SendMsg(Cmd_Mesg_IrisUp);
		};
		virtual void IrisDown()
		{
			SendMsg(Cmd_Mesg_IrisDown);
		};
		virtual void FocusUp()
		{
			SendMsg(Cmd_Mesg_FocusFar);
		};
		virtual void FocusDown()
		{
			SendMsg(Cmd_Mesg_FocusNear);
		};
		virtual void EnableIMG()
		{
			SendMsg(Cmd_Mesg_ImgEnh);
		};
		virtual void EnableAuto_Iris_Focus()  //预留
		{
			SendMsg(Cmd_Mesg_unable);
		};
		virtual void AIMPOS_X()
		{
			SendMsg(Cmd_Mesg_AIMPOS_X);
		};
		virtual void AIMPOS_Y()
		{
			SendMsg(Cmd_Mesg_AIMPOS_Y);
		};
		virtual void ZoomLongCtrl()
		{
			SendMsg(Cmd_Mesg_ZoomLong);
		};
		virtual void ZoomShortCtrl()
		{
			SendMsg(Cmd_Mesg_ZoomShort);
		};
		virtual void AXIS_X()
		{
			SendMsg(Cmd_Mesg_AXISX);
		};
		virtual void AXIS_Y()
		{
			SendMsg(Cmd_Mesg_AXISY);
		};
#endif
		void EnableTrk( )
			{
				SendMsg(Cmd_Mesg_TrkCtrl);
			};
			virtual void EnableMtd()
			{
				SendMsg(Cmd_Mesg_Mtd);
			};
			virtual void TrkBoxCtrl()
			{
				SendMsg(Cmd_Mesg_TrkBoxCtrl);
			};
			virtual void EnableTrkSearch( )
			{
				SendMsg(Cmd_Mesg_TrkSearch);
			};
			virtual void IrisUp()
			{
				SendMsg(Cmd_Mesg_IrisUp);
			};
			virtual void IrisDown( )
			{
				SendMsg(Cmd_Mesg_IrisDown);
			};
			virtual void FocusUp( )
			{
				SendMsg(Cmd_Mesg_FocusFar);
			};
			virtual void FocusDown( )
			{
				SendMsg(Cmd_Mesg_FocusNear);
			};
			virtual void EnableIMG( )
			{
				SendMsg(Cmd_Mesg_ImgEnh);
			};
			virtual void EnableMmt()
			{
				SendMsg(Cmd_Mesg_Mmt);
			};
			virtual void AIMPOS_X()
			{
				SendMsg(Cmd_Mesg_AIMPOS_X);
			};
			virtual void AIMPOS_Y()
			{
				SendMsg(Cmd_Mesg_AIMPOS_Y);
			};
			virtual void ZoomLongCtrl()
			{
				SendMsg(Cmd_Mesg_ZoomLong);
			};
			virtual void ZoomShortCtrl()
			{
				SendMsg(Cmd_Mesg_ZoomShort);
			};
			virtual void AXIS_X()
			{
				SendMsg(Cmd_Mesg_AXISX);
			};
			virtual void AXIS_Y()
			{
				SendMsg(Cmd_Mesg_AXISY);
			};
			virtual void SwitchSerson()
			{
				SendMsg(Cmd_Mesg_SensorCtrl);
			};
			virtual void PresetCtrl()
			{
				SendMsg(Cmd_Mesg_PresetCtrl);
			}
			virtual void PALImgEnh()
			{
				SendMsg(Cmd_Mesg_PALImgEnh);
			}
			virtual void EnableSelfTest()
			{
				SendMsg(Cmd_Mesg_SelfTest);
			}
			virtual void AxisMove()
			{
				SendMsg(Cmd_Mesg_AxisMove);
			}
			virtual void MainElectronicZoom()
			{
				SendMsg(Cmd_Mesg_MainElectronicZoom);
			}
			virtual void PipElectronicZoom()
			{
				SendMsg(Cmd_Mesg_PipElectronicZoom);
			}
			virtual void WorkMode()
			{
				SendMsg(Cmd_Mesg_WorkMode);
			}
			virtual void EnableOsd()
			{
				SendMsg(Cmd_Mesg_Osd);
			}
			virtual void SensorMode()
			{
				SendMsg(Cmd_Mesg_SensorMode);
			}
			virtual void TVFov()
			{
				SendMsg(Cmd_Mesg_TVFov);
			}
			virtual void PALFov()
			{
				SendMsg(Cmd_Mesg_PALFov);
			}
			virtual void Pan()
			{
				SendMsg(Cmd_Mesg_Pan);
			}
			virtual void Tilt()
			{
				SendMsg(Cmd_Mesg_Tilt);
			}
			virtual void VideoCompression()
			{
				SendMsg(Cmd_Mesg_VideoCompression);
			}
			virtual void TrkMode()
			{
				SendMsg(Cmd_Mesg_TrkMode);
			}
			virtual void CheckMode()
			{
				SendMsg(Cmd_Mesg_CheckMode);
			}
			virtual void EnhMode()
			{
				SendMsg(Cmd_Mesg_EnhMode);
			}

		int finderMapKey(int mVal);

protected:

    void  processBtnEvent(int btnIDNum);
    void processAixsEvent(int AxisIDNum);
    void getReadLocalJosCfgSettingFile(void);
	int  preinitial();
	int  findFrameHeader(void);
	int  prcRcvFrameBufQue();
	volatile unsigned int  SensorStat;
	volatile unsigned int  AvtTrkAimSize;
	u_int8_t  *procbufque;
	 uint     rcvbufofft;
	 u_int8_t *ptr;
	 vector<unsigned char>  rcvBufQue;;

	inline  void processExtEvent(pExtEventFunc  prcEventFunc)
	{
		prcEventFunc();
	}
	MAP_KYENUM_TO_PFUNC   keyNumToFunc;
	MAP_KYENUM_TO_PUTBUFFID  keyNumToID;

private:
	int  startSelfCheak();
	int  mainVedioChannelSel();
	int channelBind();
	int trackEnCmd();
	int mutileTagartNotice();
	int mutileTagartSelect();
	int   imageEnhance();
	int trackFineuning();
	int confirmAxisInfo();
	int ElectronicZoom();
	int AVTsetting();
	static int SendMsg(int MsgId)
	{
		Work_quePut(MsgId);
		return 0;
	}
	int processExtInput(u_int8_t *mExtInput,uint size);
	int check_InputDate(u_int8_t *pbuf, uint  len);
	u_int8_t  check_sum(uint len_t);
	EXT_CtrlInput     extCtrl;
};


#endif

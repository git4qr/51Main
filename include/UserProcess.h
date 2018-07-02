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

typedef struct{
	unsigned int distance:1;
	unsigned int speed:1;
	unsigned int focus:1;
	unsigned int time:1;
	unsigned int plantAgle:1;
	unsigned int targetPost:1;
	unsigned int navDuration:1;
	unsigned int commuDuration:1;
	//char 2
	unsigned int  workPlace:1;
	unsigned int softVer:1;
	unsigned int reserve0:6;

	unsigned int   focusFeedBack:16;
	unsigned int   speedInfo:8;
	unsigned int   yearInfo:8;
	unsigned int   monthInfo:8;
	unsigned int   dayInfo:8;
	unsigned int   timeHours:8;
	unsigned int  timeMini:8;
	unsigned int  timeSecnd:8;
	unsigned int plantTilt1:8;
	unsigned int plantTilt2:8;
	unsigned int plantPan:16;
	unsigned int lgitudeAgl:8;
	unsigned int lgitudeMini:8;
	unsigned int lgitudeSencd:8;
  	unsigned int latitudeAgl:8;
	unsigned int latitudeMini:8;
	unsigned int latitudeSencd:8;
	unsigned int distanceInfo:16;
	unsigned int navDuratInfo1:8;
	unsigned int navDuratInfo2:8;
	unsigned int commuDuratInfo:16;
}OSD_INFO;

#define   OSD_INFO_SIZE (27)

typedef struct{
	uint channel0:1;
	uint channel1:1;
	uint channel2:1;
	uint channel3:1;
	uint channel4:1;
	uint channel5:1;
	uint reserve5:2;
}channelTable;

#define 	MAXSENDSIZE (15)
typedef struct {
	   int byteSizeSend;
	   unsigned char sendBuff[MAXSENDSIZE];
}sendInfo;


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
	float    confitData;
}systemSetting;

class CUserBase
{
public:
	CUserBase();
	virtual ~CUserBase();
	int *ExtInputCtrl;
	vector<int> ExtInputCtrlValue;
	virtual int  Create(){return 0;};
	virtual int Init(){return 0;};
	virtual int  Config(){return 0;};
	void PreInit();
	virtual int Run(){return 0;};
	virtual int  Stop(){return 0;};
	int process_ExtInputData( u_int8_t *data_buf, uint data_len);
	static int *EXT_Ctrl;
	float *Host_Ctrl;
	int feedback;
	float ACK_read;
	IMGSTATUS  avt_status;
	int errorOutPut[2];
	int mainProStat[20];
	systemSetting avtSetting;
	OSD_INFO   osd_Param;
	virtual int  Destroy(){
		delete EXT_Ctrl;
		delete Host_Ctrl;
		return 0;
	};
		void EnableMmtSelect()
		{
			SendMsg(Cmd_Mesg_MmtSelect);
		}
		void EnableOSD()
		{
		SendMsg(Cmd_Mesg_Osd);
		}
		void Enableconfig()
		{
			SendMsg(Cmd_Mesg_config_Write);
		};
		void EnableSavePro()
		{
			SendMsg(Cmd_Mesg_config_Write_Save);
		};
		void EnableTrk( )
			{
				SendMsg(Cmd_Mesg_TrkCtrl);
			};
			void EnableMtd()
			{
				SendMsg(Cmd_Mesg_Mtd);
			};
			void ENableIrisAndFocusAndExit()
			{
				SendMsg(Cmd_Mesg_IrisAndFocusAndExit);
			};
			 void EnableTrkSearch( )
			{
				SendMsg(Cmd_Mesg_TrkSearch);
			};
			 void EnableFuncMenu()
			{
				SendMsg(Cmd_Mesg_FuncMenu);
			};
			 void IrisDown( )
			{
				SendMsg(Cmd_Mesg_IrisDown);
			};
			 void FocusUp( )
			{
				SendMsg(Cmd_Mesg_FocusFar);
			};
			 void FocusDown( )
			{
				SendMsg(Cmd_Mesg_FocusNear);
			};
			 void EnableIMG( )
			{
				SendMsg(Cmd_Mesg_ImgEnh);
			};
			 void EnableMmt()
			{
				SendMsg(Cmd_Mesg_Mmt);
			};
			 void AIMPOS_X()
			{
				SendMsg(Cmd_Mesg_AIMPOS_X);
			};
			 void AIMPOS_Y()
			{
				SendMsg(Cmd_Mesg_AIMPOS_Y);
			};
			 void ZoomLongCtrl()
			{
				SendMsg(Cmd_Mesg_ZoomLong);
			};
			 void ZoomShortCtrl()
			{
				SendMsg(Cmd_Mesg_ZoomShort);
			};
			 void AXIS_X()
			{
				SendMsg(Cmd_Mesg_AXISX);
			};
			 void AXIS_Y()
			{
				SendMsg(Cmd_Mesg_AXISY);
			};
			void SwitchSensor()
			{
				SendMsg(Cmd_Mesg_SensorCtrl);
			};
			 void PresetCtrl()
			{
				SendMsg(Cmd_Mesg_PresetCtrl);
			}
			void PALImgEnh()
			{
				//SendMsg(Cmd_Mesg_PALImgEnh);
			}
			 void EnableSelfTest()
			{
				SendMsg(Cmd_Mesg_SelfTest);
			}
			 void EnableChannel_binding()
			 {
				 SendMsg(Cmd_Mesg_Channel_binding);
			 }
			 void AxisMove()
			{
				SendMsg(Cmd_Mesg_AxisMove);
			}
			 void MainElectronicZoom()
			{
				SendMsg(Cmd_Mesg_MainElectronicZoom);
			}
			 void PipElectronicZoom()
			{
				SendMsg(Cmd_Mesg_PipElectronicZoom);
			}
			 void WorkMode()
			{
				SendMsg(Cmd_Mesg_WorkMode);
			}
			 void SensorMode()
			{
				SendMsg(Cmd_Mesg_SensorMode);
			}
			 void TVFov()
			{
				SendMsg(Cmd_Mesg_TVFov);
			}
			 void PALFov()
			{
				SendMsg(Cmd_Mesg_PALFov);
			}
			 void Pan()
			{
				SendMsg(Cmd_Mesg_Pan);
			}
			void Tilt()
			{
				SendMsg(Cmd_Mesg_Tilt);
			}
			 void VideoCompression()
			{
				SendMsg(Cmd_Mesg_VideoCompression);
			}
			 void TrkMode()
			{
				SendMsg(Cmd_Mesg_TrkMode);
			}
			 void CheckMode()
			{
				SendMsg(Cmd_Mesg_CheckMode);
			}
			 void EnhMode()
			{
				SendMsg(Cmd_Mesg_EnhMode);
			}
			 void ReadProfile()
			 {
				 SendMsg(Cmd_IPC_Config);
			 }
			 void ReadShmOSD()
			 {
				 SendMsg(Cmd_IPC_OSD);
			 }
			 void ReadShmUTC()
			 {
				 SendMsg(Cmd_IPC_UTC);
			 }
			 void ReadShmCamera()
			 {
				 SendMsg(Cmd_IPC_Camera);
			 }
			 void EnableElectronicZoom()
			 {
				 SendMsg(Cmd_Mesg_MainElectronicZoom);
			 }
			 void EnablesaveAxis()
			 {
				 SendMsg(Cmd_Mesg_saveAxis);
			 }

			 void EnablePicp()
			 {
				 SendMsg(Cmd_Mesg_Picp);
			 }
			 void EnableswitchVideoChannel()
			 {
				 SendMsg(Cmd_Mesg_switchVideoChannel);
			 }
			 void EnableframeCtrl()
			 {
				 SendMsg(Cmd_Mesg_frameCtrl);
			 }
			 void Enablecompression_quality()
			 {
				 SendMsg(Cmd_Mesg_compression_quality);
			 }
			 void EnablewordColor()
			 {
				 SendMsg(Cmd_Mesg_wordColor);
			 }
			 void EnablewordType()
			 {
				 SendMsg(Cmd_Mesg_wordType);
			 }
			 void EnablewordSize()
			 {
				 SendMsg(Cmd_Mesg_wordSize);
			 }
			 void WordDisEnable()
			 {
				 SendMsg(Cmd_Mesg_wordDisEnable);
			 }
			 void Config_Write_Save()
			 {
				 SendMsg(Cmd_Mesg_config_Write);
			 }
			 void Config_Read()
			 {
				 SendMsg(Cmd_Mesg_config_Read);
			 }
			 void EnableJos_kboard()
			 {
				 SendMsg(Cmd_Mesg_jos_kboard);
			 }

		int finderMapKey(int mVal);

protected:

    void  processBtnEvent(int btnIDNum);
    void processAixsEvent(int AxisIDNum);
    void getReadLocalJosCfgSettingFile(void);
	int  preinitial();
	int  findFrameHeader(void);
	int  prcRcvFrameBufQue();
	int getSendInfo(int  respondId, sendInfo * psendBuf);
	volatile unsigned int  SensorStat;
	volatile unsigned int  AvtTrkAimSize;
	u_int8_t  *procbufque;
	 uint     rcvbufofft;
	 u_int8_t *ptr;
	 vector<unsigned char>  rcvBufQue;

	inline  void processExtEvent(pExtEventFunc  prcEventFunc)
	{
		prcEventFunc();
	}
	MAP_KYENUM_TO_PFUNC   keyNumToFunc;
	MAP_KYENUM_TO_PUTBUFFID  keyNumToID;
    sendInfo repSendBuffer;
private:
	    void   startSelfCheak();
		void  mainVedioChannelSel();
		void  channelBind();
		void trackEnCmd();
		void  mutileTagartNotice();
		void  mutileTagartSelect();
		void    imageEnhance();
		void  trackFinetuning();
		void  confirmAxisInfo();
		void  ElectronicZoomCtrl();
		void trackSearch();
		void confirmAxisSave();
		void  moveTargetDetected();
		void pictrueInPicture();
	   void  selectVedisTransChannel();
	   void  frameFrequenceCtrl();
	   void  vedioCompressQuality();
	   void  opticalZoomCtrl();
	   void irisCtrl();
	   void foucsCtrl();
	   void fontColor();
	   void fontStyle();
	   void fontSize();
	   void  fontDisplayCtrl();
	   void configSetting();
	   void readCurrentSetting();
	   void extExtraInputCtrl();
	   void extFocusInputCtrl();
       void saveParameter();
       void  osdDisplay();
          //response  receive  command
   void  startCheckAnswer(sendInfo * spBuf);
   void mainVedioChannel(sendInfo * spBuf);
   void bindVedioChannel(sendInfo * spBuf);
   void trackStatus(sendInfo * spBuf);
   void trackTypes(sendInfo * spBuf);
   void trackErrOutput(sendInfo * spBuf);
   void mutilTargetNoticeStatus(sendInfo * spBuf);
   void multilTargetNumSelectStatus(sendInfo * spBuf);
   void  imageEnhanceStatus(sendInfo * spBuf);
   void trackFinetuningStat(sendInfo * spBuf);
   void confirmAxisStat(sendInfo * spBuf);
   void ElectronicZoomStat(sendInfo * spBuf);
   void trackSearchStat(sendInfo * spBuf);
   void moveTargetDetectedStat(sendInfo * spBuf);
   void pictureInPictureStat(sendInfo * spBuf);
   void  vedioTransChannelStat(sendInfo * spBuf);
   void frameFrequenceStat(sendInfo * spBuf);
   void vedioCompressStat(sendInfo * spBuf);
   void settingCmdRespon(sendInfo * spBuf);
   void readConfigSetting(sendInfo * spBuf);
   void extExtraInputResponse(sendInfo * spBuf);

	static int SendMsg(int MsgId)
	{
		Work_quePut(MsgId);
		return 0;
	}
	int processExtInput(u_int8_t *mExtInput,uint size);
	int check_InputDate(u_int8_t *pbuf, uint  len);
	u_int8_t  check_sum(uint len_t);
        u_int8_t  sendCheck_sum(uint len, u_int8_t *tmpbuf);
	EXT_CtrlInput     extCtrl;
};


#endif

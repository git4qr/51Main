#include <string.h>
#include <algorithm>
#include <vector>
#include <map>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "UserProcess.h"
#include "finder.h"

using namespace cv;
using namespace std;

#define   btnsettingcfgfilesize   (10)
#define  KeyMAXSize  (12)
#define   MAX_CFG_ID  (19)
static int  cfg_blk_val[16];
static u_int8_t  frame_head[]={0xEB, 0x53};
//static  pExtEventFunc  funcArray[]={NULL,track,SelSensor,TrkBoxCtrl,TrkSearch,upIris,downIris,focusUp,FocusDown,imgEnhance,autoIsriFocus,
															//	aimposx,aimposy,zoomlong, zoomshort, axisXctrl,axisYctrl };

int * CUserBase::EXT_Ctrl = new int[50];


void CUserBase::getReadLocalJosCfgSettingFile(void)
{
	string cfgAvtFile= "LocalBtnSetting.yml";
	char cfg_avt[16] = "btn_key_";
	int configId_Max=KeyMAXSize;
	FILE *fp = fopen(cfgAvtFile.c_str(), "rt");
	int tmp;
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		if(len < btnsettingcfgfilesize)  printf("INFO: setting file lenth Error\r\n:");
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::READ);
			if(fr.isOpened())
			{
				for(int i=1; i<configId_Max; i++){
					sprintf(cfg_avt, "btn_key_%d", i);
					tmp=cfg_blk_val[i] = (int)fr[cfg_avt];
					keyNumToID.insert(map<int, int> :: value_type(i,cfg_blk_val[i]));
					//printf(" update cfg [%d] %d\r \n", i, cfg_blk_val[i]);
			}
 				fr.release();
			}
		}
	}
}

CUserBase::CUserBase()
{
	ExtInputCtrl = new int[20];
	ExtInputCtrlValue.assign(128,0);
	rcvbufofft=0;
	procbufque = new  u_int8_t[RCVBUFFSIZELENTH];
	memset(procbufque,0,sizeof(procbufque));
 	memset(ExtInputCtrl, 0, sizeof(int) * 20);
 	memset(&extCtrl,0,sizeof(extCtrl));
 	memset(&avtSetting,0,sizeof(avtSetting));
 	ptr=&procbufque[0];
 	memset(EXT_Ctrl, 0, sizeof(int) * 50);
	if(Host_Ctrl == NULL){
	Host_Ctrl = new float[30];
	memset(Host_Ctrl, 0, sizeof(float) * 30);
	}
}

CUserBase::~CUserBase()
{
	Destroy();
}

void CUserBase::PreInit()
{

}

int CUserBase::preinitial()
{
	  int i;
	  getReadLocalJosCfgSettingFile();
	//  for( i=0;i<MAX_CFG_ID;i++){
	//	  keyNumToFunc.insert(pair<int, pExtEventFunc>(cfg_blk_val[i],funcArray[i]));
		//  keyNumToID.insert(pair<int, int >(cfg_blk_val[i],i));
	 // }
    return 0;
}

int   CUserBase::finderMapKey(int mVal)
{

	int n = -1;
#if 1
	    MAP_KYENUM_TO_PUTBUFFID::iterator it = find_if(keyNumToID.begin(), keyNumToID.end(), finder(mVal));
	    if (it != keyNumToID.end())
	    {
	        n = (*it).first;
	    }
	    return n;
#else
	        int s = mVal;
	        MAP_KYENUM_TO_PUTBUFFID::iterator find_item = find_if(keyNumToID.begin(), keyNumToID.end(),
	        [s](map<int,int>::value_type item)
	    {
	        return item.second == s;
	    });

	    if (find_item!= keyNumToID.end())
	    {
	        n = (*find_item).first;
	    }

	    return n;
#endif
}

int  CUserBase::findFrameHeader(void)
{
	int  frameVaild=0;
	for (size_t i = 0; i < rcvBufQue.size(); i++)
	{
	   //if(frame_head[0]==rcvbufque.at(i)&&frame_head[1]==rcvbufque.at(i+1)) return i;
		if(!frameVaild&&frame_head[0]==rcvBufQue.at(i)){
			     frameVaild=1;
			     continue;
		}
		if(frameVaild&&frame_head[1]==rcvBufQue.at(i))  return (i-1);
		else  {
			frameVaild=0;
			continue;
		}
	}
	return -1;
}

u_int8_t  CUserBase::check_sum(uint len_t)
{
	u_int8_t cksum = 0;
	int lenth=len_t-4;
	for(int n=3; lenth>0; lenth--,n++)
	{
		cksum ^= rcvBufQue.at(n);
	}
	return  cksum;
}


void CUserBase::startSelfCheak()
{
//	printf("INFO: startSelfCheak\r\n");
	int startCheck=rcvBufQue.at(4);
	Host_Ctrl[selfTest]=startCheck;
	EnableSelfTest();
}
void CUserBase:: mainVedioChannelSel()
{
	//printf("INFO: mainVedioChannelSel\r\n");
	int mainVedioChannel=rcvBufQue.at(4);
	Host_Ctrl[mainVideo]=mainVedioChannel;
	SwitchSensor();
}

void CUserBase::channelBind()
{
//	printf("INFO: channelBind\r\n");
	int channelBind=rcvBufQue.at(4);
	Host_Ctrl[Channel_binding]=channelBind;
}

void CUserBase::trackEnCmd()
{
//	printf("INFO: track\r\n");
	int trackEnCmd= rcvBufQue.at(4);
	EXT_Ctrl[Cmd_Mesg_TrkCtrl-1]=trackEnCmd;
	EnableTrk();
}

void CUserBase::mutileTagartNotice()
{
//    printf("INFO: enable mutile target!!\r\n");
    int mutiltargetNotice=rcvBufQue.at(4);
    EXT_Ctrl[Cmd_Mesg_Mmt-1]=mutiltargetNotice;
    printf("CUserBase=======>EXT_Ctrl = %p\n", EXT_Ctrl);
    EnableMmt();
}


void CUserBase::mutileTagartSelect()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
                             EnableTrk( );
            	             break;
            case   0x2:
            	              EnableMtd();
            	             break;
            case   0x3:
            	           ZoomLongCtrl();
            	             break;
            case   0x4:
            	            ZoomShortCtrl();
            	              break;
            case   0x5:
            	             //TrkBoxCtrl();
            	             break;
         }
}

void   CUserBase::imageEnhance()
{
	//  printf("INFO: image enhance \r\n");
	  int imageEnhance=rcvBufQue.at(4);
	  EXT_Ctrl[Cmd_Mesg_ImgEnh-1]=imageEnhance;
	  EnableIMG();
}

void CUserBase::trackFinetuning()
{
    EXT_Ctrl[Cmd_Mesg_AIMPOS_X-1]=0;
    EXT_Ctrl[Cmd_Mesg_AIMPOS_Y-1]=0;
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	         	 EXT_Ctrl[Cmd_Mesg_AIMPOS_X-1]=1;
            	             AIMPOS_X();

            	             break;
            case   0x2:
            	            EXT_Ctrl[Cmd_Mesg_AIMPOS_X-1]=2;
	                          AIMPOS_X();

	                          break;
            default:
            	            EXT_Ctrl[Cmd_Mesg_AIMPOS_X-1]=0;
         }
    switch(rcvBufQue.at(5) ){
    		case   0x1:
    						EXT_Ctrl[Cmd_Mesg_AIMPOS_Y-1]=1;
    								AIMPOS_Y();

    									break;
    		case   0x2:
    						EXT_Ctrl[Cmd_Mesg_AIMPOS_Y-1]=2;
                    			AIMPOS_Y();

                    			break;
    		default:
    						EXT_Ctrl[Cmd_Mesg_AIMPOS_Y]=0;
    }
}

void CUserBase::confirmAxisInfo()
{
    Host_Ctrl[moveAxisX]=0;
    Host_Ctrl[moveAxisY]=0;
    switch(rcvBufQue.at(4) ){
            case   0x1:
          	            Host_Ctrl[moveAxisX]=1;
            	             break;
            case   0x2:
            	            Host_Ctrl[moveAxisX]=2;
            	            break;
            default:
            	            Host_Ctrl[moveAxisX]=0;
         }
    switch(rcvBufQue.at(5) ){
    		case   0x1:
    			            Host_Ctrl[moveAxisY]=1;
    									break;
    		case   0x2:
    			             Host_Ctrl[moveAxisY]=2;
    			             break;
    		default:
    			             Host_Ctrl[moveAxisY]=0;
    }
    AxisMove();
}

void CUserBase::ElectronicZoomCtrl()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	Host_Ctrl[ElectronicZoom]=2;
            	             break;
            case   0x2:
            	Host_Ctrl[ElectronicZoom]=4;
	                          break;
            case   0x3:
            	Host_Ctrl[ElectronicZoom]=8;
                        	 break;
            case   0x4:
            	Host_Ctrl[ElectronicZoom]=0;
            	            break;
    }
    MainElectronicZoom();
}

void CUserBase::trackSearch()
{
	int tarkSearkSwitch,trakSearchx,trackSearchy;
	trakSearchx=trackSearchy=0;
	tarkSearkSwitch=rcvBufQue.at(4);
	trakSearchx=  (rcvBufQue.at(5)|rcvBufQue.at(6)<<8);
	trackSearchy=  (rcvBufQue.at(7)|rcvBufQue.at(8)<<8);
	EXT_Ctrl[Cmd_Mesg_AXISX-1] =trakSearchx;
     EXT_Ctrl[Cmd_Mesg_AXISY-1] =trackSearchy;
     EnableTrkSearch( );
}

void CUserBase::confirmAxisSave()
{
	  EnablesaveAxis();
}

void CUserBase::moveTargetDetected()
{
	int moveTargetDetected =rcvBufQue.at(4);
	  ExtInputCtrl[Cmd_Mesg_Mtd-1]=moveTargetDetected;
	  EnableMtd();

}
void CUserBase::pictrueInPicture()
{
	int pictrueInPicture =rcvBufQue.at(4);
	int pipChannel =rcvBufQue.at(5);
	Host_Ctrl[picp]=pictrueInPicture;
	Host_Ctrl[picpChannel]=pipChannel;
	EnablePicp();
}

void CUserBase::selectVedisTransChannel()
{
     int  selTransChannel=rcvBufQue.at(4);
 	Host_Ctrl[switchVideoChannel]=selTransChannel;
  	EnableswitchVideoChannel();
}

void CUserBase::frameFrequenceCtrl()
{
	 int  frameFrequence=rcvBufQue.at(4);
	 int  channelNum=rcvBufQue.at(5);
	Host_Ctrl[frameCtrl]=frameFrequence;
	Host_Ctrl[frameChannel]=channelNum;
	EnableframeCtrl();
}

void CUserBase::vedioCompressQuality()
{
	 int  compressQualty=rcvBufQue.at(4);
	 int compresschanlNum=rcvBufQue.at(5);
	 Host_Ctrl[compression_quality]=compressQualty;
	 Host_Ctrl[compressionChannel]=compresschanlNum;
	 Enablecompression_quality();
}
void CUserBase::opticalZoomCtrl()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	EXT_Ctrl[Cmd_Mesg_ZoomShort-1]=1;
            	ZoomShortCtrl();
            	             break;
            case   0x2:
            	EXT_Ctrl[Cmd_Mesg_ZoomLong-1]=1;
            	ZoomLongCtrl();
            	   break;
       }
}

void  CUserBase::irisCtrl()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	EXT_Ctrl[Cmd_Mesg_IrisDown-1]=1;
            	IrisDown();
            	             break;
            case   0x2:
            	//EXT_Ctrl[Cmd_Mesg_IrisUp-1]=1;
            	//IrisUp();
            	   break;
       }
}

void CUserBase::foucsCtrl()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	EXT_Ctrl[Cmd_Mesg_FocusNear-1]=1;
            	FocusDown();
            	             break;
            case   0x2:
            	EXT_Ctrl[Cmd_Mesg_FocusFar-1]=1;
            	FocusUp();
            	   break;
       }
}
void CUserBase::fontColor()
{
	int  fontColor=rcvBufQue.at(4);
	Host_Ctrl[wordColor]=fontColor;
	EnablewordColor();
}
void CUserBase::fontStyle()
{
	int  fontStyle=rcvBufQue.at(4);
	Host_Ctrl[wordType]=fontStyle;
	 EnablewordType();
}
void CUserBase::fontSize()
{
	int  fontSize=rcvBufQue.at(4);
	Host_Ctrl[wordSize]=fontSize;
	EnablewordSize();
}
void  CUserBase::fontDisplayCtrl()
{
	int  fontDisplayCtrl=rcvBufQue.at(4);
	Host_Ctrl[wordDisEnable]=fontDisplayCtrl;
	WordDisEnable();
}
void  CUserBase::configSetting()
{
	uint8_t  tempbuf[4];
	memset(&avtSetting,0,sizeof(avtSetting));
	avtSetting.cmdBlock=rcvBufQue.at(4) ;
	avtSetting.cmdFiled=rcvBufQue.at(5) ;
	for(int m=6;m<10;m++)
		tempbuf[m-6]=rcvBufQue.at(m);
	memcpy(&avtSetting.confitData,tempbuf,sizeof(float));
	//printf("INFO:  Block=%d, filed=%d, value=%f\r\n",avtSetting.cmdBlock,avtSetting.cmdFiled,avtSetting.confitData);
	Host_Ctrl[config_Wblock]=	avtSetting.cmdBlock;
	Host_Ctrl[config_Wfield]=avtSetting.cmdFiled;
	Host_Ctrl[config_Wvalue]=avtSetting.confitData;
	Config_Write_Save();
}

void CUserBase::readCurrentSetting()
{
	systemSetting currtSetting;
	memset(&currtSetting,0,sizeof(currtSetting));
	currtSetting.cmdBlock=rcvBufQue.at(4);
	currtSetting.cmdFiled=rcvBufQue.at(5);
	Host_Ctrl[config_Rblock]=	currtSetting.cmdBlock;
	Host_Ctrl[config_Rfield]=currtSetting.cmdFiled;
	Config_Read();
}

void   CUserBase::extExtraInputCtrl()
{

}

void CUserBase::extFocusInputCtrl()
{
    int extFocus=-1;
    extFocus=  (rcvBufQue.at(4)|rcvBufQue.at(5)<<8);

}
void CUserBase::saveParameter()
{
   EnableSavePro();

}
void  CUserBase::osdDisplay()
{
	  uint8_t tempbuf[OSD_INFO_SIZE];
	  int n;
      memset(&osd_Param,0,sizeof(osd_Param));
      for(n=0;n<OSD_INFO_SIZE;n++)
    	  tempbuf[n]=rcvBufQue.at(n+4);
      memcpy(&osd_Param,tempbuf,OSD_INFO_SIZE);

      printf("UserProcess ====> speedinfo = %d\n", osd_Param.speedInfo);
      printf("UserProcess ====> distance = %d\n", osd_Param.distanceInfo);
      printf("UserProcess ====> speed = %d\n", osd_Param.speed);
      printf("UserProcess ====> distance = %d\n", osd_Param.distanceInfo);

      EnableOSD();
}

int  CUserBase::prcRcvFrameBufQue()
{
    int ret =  -1;
  // printf("INFO: prcocee Buff queue!!\r\n");
	int  cmdLength= rcvBufQue.at(2);
	if(cmdLength<5) {
		printf("Worning::  Invaild frame\r\n");
		 rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+cmdLength);  //  analysis complete erase bytes
		return ret;
	}
	u_int8_t checkSum = check_sum(cmdLength);
//	printf("INFO: checksum %02x\r\n",checkSum);
	if(checkSum==rcvBufQue.at(cmdLength-1))
	{
       switch(rcvBufQue.at(3)){
                case    0x01:
                							startSelfCheak();
                	       break;
                case  0x02:
                	                       mainVedioChannelSel();
                            break;
                case    0x03:
                							channelBind();
                	       break;
                case  0x04:
                							trackEnCmd();
                            break;
                case    0x05:
                							mutileTagartNotice();
                	       break;
                case  0x06:
                	                        mutileTagartSelect();
                            break;
                case    0x07:
                	                        imageEnhance();
                	       break;
                case  0x08:
                							trackFinetuning();
                            break;
                case    0x09:
                							confirmAxisInfo();
                	       break;
                case    0x0a:
                							ElectronicZoomCtrl();
                	       break;
                case  0x0b:
                							trackSearch();
                            break;
                case    0x0c:
                							confirmAxisSave();
                	       break;
                case  0x0d:
                							moveTargetDetected();
                            break;
                case    0x0e:
                							pictrueInPicture();
                	       break;
                case    0x0f:
                							selectVedisTransChannel();
                            break;
                case    0x10:
                							 frameFrequenceCtrl();
                	       break;
                case  0x11:
                                             vedioCompressQuality();
                            break;
                case    0x12:
                	                         opticalZoomCtrl();
                            break;
                case    0x13:
                					          irisCtrl();
                	       break;
                case   0x14:
                							  foucsCtrl();
                            break;
                case    0x20:
                							  fontColor();
                	       break;
                case  0x21:
                							  fontStyle();
                            break;
                case    0x22:
                							 fontSize();
                	       break;
                case    0x23:
                							 osdDisplay();
                	       break;
                case  0x24:
                		                     fontDisplayCtrl();
                            break;
                case    0x30:
                	                         configSetting();
                	       break;
                case  0x31:
                                             readCurrentSetting();
                            break;
                case  0x32:
                							extExtraInputCtrl();
                            break;
                case  0x34:
                							saveParameter();
                            break;

                case  0x40:
                							extFocusInputCtrl();
                            break;
                default:
                	        printf("INFO: Unknow  Control Command, please check!!!\r\n ");
                	 	 	ret =0;
                            break;
             }
	  }
	 rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+cmdLength);  //  analysis complete erase bytes
       return 1;
}

//********************************AVT to host ****************************
// ***************send****************

u_int8_t CUserBase:: sendCheck_sum(uint len, u_int8_t *tmpbuf)
{
	u_int8_t  ckeSum=0;
     for(int n=0;n<len;n++)
    	 ckeSum ^= tmpbuf[n];
    return ckeSum;
}

void  CUserBase:: startCheckAnswer(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  answerStarkCkArry[4];
	answerStarkCkArry[0]=0x01;
	answerStarkCkArry[1]=0x01;
	answerStarkCkArry[2]=0x02;
	sumCheck=sendCheck_sum(3,answerStarkCkArry);
	answerStarkCkArry[4]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x07;
	memcpy(&(spBuf->sendBuff[3]),answerStarkCkArry,4);
}

void  CUserBase:: mainVedioChannel(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  mainVedioChannel[3];
	mainVedioChannel[0]=0x04;
	mainVedioChannel[1]=avt_status.SensorStat;
	mainVedioChannel[2]= sendCheck_sum(2,mainVedioChannel);
	//mainVedioChannel[2]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),mainVedioChannel,3);
}

void  CUserBase:: bindVedioChannel(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  bindVedioChannel[3];
	bindVedioChannel[0]=0x05;
	bindVedioChannel[1]=0x7;
	sumCheck=sendCheck_sum(2,bindVedioChannel);
	bindVedioChannel[2]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),bindVedioChannel,3);
}

void  CUserBase:: trackStatus(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  trackStatus[3];
	trackStatus[0]=0x06;
	trackStatus[1]=avt_status.AvtTrkStat;
	sumCheck=sendCheck_sum(2,trackStatus);
	trackStatus[2]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),trackStatus,3);
}

void  CUserBase:: trackTypes(sendInfo * spBuf)
{
	int currtrackTypes;
	u_int8_t sumCheck;
	u_int8_t  trackTypes[3];
	trackTypes[0]=0x07;
	trackTypes[1]=avt_status.TrkStat;
	sumCheck=sendCheck_sum(2,trackTypes);
	trackTypes[2]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),trackTypes,3);
}

void  CUserBase:: trackErrOutput(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  trackErrOutput[6];
	trackErrOutput[0]=0x08;
	trackErrOutput[1]=errorOutPut[0]&0xff; //errorOutPut[0]
	trackErrOutput[2]=(errorOutPut[0]>>8)&0xff; //errorOutPut[0]
	trackErrOutput[3]= errorOutPut[1]&0xff; //errorOutPut[1]
	trackErrOutput[4]=(errorOutPut[1]>>8)&0xff; //errorOutPut[1]
	sumCheck=sendCheck_sum(5,trackErrOutput);
	trackErrOutput[5]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x09;
	memcpy(&(spBuf->sendBuff[3]),trackErrOutput,6);
}

void  CUserBase:: mutilTargetNoticeStatus(sendInfo * spBuf)
{
	channelTable    mutilTargetSta;
	u_int8_t sumCheck;
	mutilTargetSta.channel0=avt_status.MtdState[0];
	mutilTargetSta.channel1=avt_status.MtdState[1];
	mutilTargetSta.channel2=avt_status.MtdState[2];
	mutilTargetSta.channel3=avt_status.MtdState[3];
	mutilTargetSta.channel4=avt_status.MtdState[4];
	mutilTargetSta.channel5=avt_status.MtdState[5];
	u_int8_t  mutilTargetNotice[3];
	mutilTargetNotice[0]=0x09;
	mutilTargetNotice[1]=*(u_int8_t*)(&mutilTargetSta);
	sumCheck=sendCheck_sum(2,mutilTargetNotice);
	mutilTargetNotice[2]=sumCheck;
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),mutilTargetNotice,3);
}

void CUserBase::multilTargetNumSelectStatus(sendInfo * spBuf)
{

	u_int8_t sumCheck;
	u_int8_t  mutilTargetSelectNum[3];
	mutilTargetSelectNum[0]=0x0a;
	mutilTargetSelectNum[1]=(u_int8_t) (mainProStat[ACK_mmtSelect_value]&0xff);
	sumCheck=sendCheck_sum(2,mutilTargetSelectNum);
	mutilTargetSelectNum[2]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),mutilTargetSelectNum,3);
}

void  CUserBase::imageEnhanceStatus(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	channelTable    enhanceChannelSta;
	enhanceChannelSta.channel0=avt_status.ImgEnhStat[0];
	enhanceChannelSta.channel1=avt_status.ImgEnhStat[1];
	enhanceChannelSta.channel2=avt_status.ImgEnhStat[2];
	enhanceChannelSta.channel3=avt_status.ImgEnhStat[3];
	enhanceChannelSta.channel4=avt_status.ImgEnhStat[4];
	enhanceChannelSta.channel5=avt_status.ImgEnhStat[5];
	u_int8_t  imageEnhance[3];
	imageEnhance[0]=0x0b;
	imageEnhance[1]=*(u_int8_t*)(&enhanceChannelSta);;
	sumCheck=sendCheck_sum(2,imageEnhance);
	imageEnhance[2]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),imageEnhance,3);

}

void CUserBase::trackFinetuningStat(sendInfo * spBuf)
{
	int currtTrackFinetuning[2]={0};
	u_int8_t sumCheck;
	u_int8_t  trackFinetuningStat[4];
	trackFinetuningStat[0]=0x0c;
	trackFinetuningStat[1]=(u_int8_t) (mainProStat[ACK_posMove_value]&0xff);
	trackFinetuningStat[2]=(u_int8_t) (mainProStat[ACK_posMove_value+1]&0xff);
	sumCheck=sendCheck_sum(3,trackFinetuningStat);
	trackFinetuningStat[3]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x07;
	memcpy(&(spBuf->sendBuff[3]),trackFinetuningStat,4);

}

void CUserBase::confirmAxisStat(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  confirmAxisStat[4];
	confirmAxisStat[0]=0x0d;
	confirmAxisStat[1]=(u_int8_t) (mainProStat[ACK_moveAxis_value]&0xff);
	confirmAxisStat[2]=(u_int8_t) (mainProStat[ACK_moveAxis_value+1]&0xff);
	sumCheck=sendCheck_sum(3,confirmAxisStat);
	confirmAxisStat[3]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x07;
	memcpy(&(spBuf->sendBuff[3]),confirmAxisStat,4);

}

void CUserBase::ElectronicZoomStat(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  ElectronicZoom[3];
	ElectronicZoom[0]=0x0e;
	ElectronicZoom[1]=(u_int8_t) (mainProStat[ACK_ElectronicZoom_value]&0xff);
	sumCheck=sendCheck_sum(2, ElectronicZoom);
	ElectronicZoom[2]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),ElectronicZoom,4);
}

void CUserBase::trackSearchStat(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  trackSearch[3];
	trackSearch[0]=0x0f;
	trackSearch[1]=(u_int8_t) (mainProStat[ACK_TrkSearch_value]&0xff);
	sumCheck=sendCheck_sum(2, trackSearch);
	trackSearch[2]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),trackSearch,3);
}

void CUserBase::moveTargetDetectedStat(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  moveTargetDetect[3];
	channelTable    currtmoveTargetDet;
	currtmoveTargetDet.channel0=avt_status.MtdState[0];
	currtmoveTargetDet.channel1=avt_status.MtdState[1];
	currtmoveTargetDet.channel2=avt_status.MtdState[2];
	currtmoveTargetDet.channel3=avt_status.MtdState[3];
	currtmoveTargetDet.channel4=avt_status.MtdState[4];
	currtmoveTargetDet.channel5=avt_status.MtdState[5];
	moveTargetDetect[0]=0x10;
	moveTargetDetect[1]=*(u_int8_t*)(&currtmoveTargetDet);
	sumCheck=sendCheck_sum(2, moveTargetDetect);
	moveTargetDetect[2]=sumCheck;
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),moveTargetDetect,3);
}

void CUserBase::pictureInPictureStat(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  pictureInPicture[4];
	channelTable    currtPiPChanneStat;
	currtPiPChanneStat.channel0=avt_status.MtdState[0];
	currtPiPChanneStat.channel1=avt_status.MtdState[1];
	currtPiPChanneStat.channel2=avt_status.MtdState[2];
	currtPiPChanneStat.channel3=avt_status.MtdState[3];
	currtPiPChanneStat.channel4=avt_status.MtdState[4];
	currtPiPChanneStat.channel5=avt_status.MtdState[5];
	pictureInPicture[0]=0x11;
	pictureInPicture[1]=avt_status.PicpSensorStat;
	pictureInPicture[2]=*(u_int8_t*)(&currtPiPChanneStat);
	sumCheck=sendCheck_sum(3, pictureInPicture);
	pictureInPicture[3]=sumCheck;
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x07;
	memcpy(&(spBuf->sendBuff[3]),pictureInPicture,4);
}

void  CUserBase::vedioTransChannelStat(sendInfo * spBuf)
{
	int vedioTransChannelStat;
	u_int8_t sumCheck;
	u_int8_t  vedioTransChannel[3];
	channelTable    currtVedioTransChnel;
	currtVedioTransChnel.channel0=avt_status.ImgVideoTrans[0];
	currtVedioTransChnel.channel1=avt_status.ImgVideoTrans[1];
	currtVedioTransChnel.channel2=avt_status.ImgVideoTrans[2];
	currtVedioTransChnel.channel3=avt_status.ImgVideoTrans[3];
	currtVedioTransChnel.channel4=avt_status.ImgVideoTrans[4];
	currtVedioTransChnel.channel5=avt_status.ImgVideoTrans[5];
	vedioTransChannel[0]=0x20;
	vedioTransChannel[1]=*(u_int8_t*)(&currtVedioTransChnel);
	sumCheck=sendCheck_sum(2, vedioTransChannel);
	vedioTransChannel[2]=sumCheck;
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x06;
	memcpy(&(spBuf->sendBuff[3]),vedioTransChannel,3);

}

void CUserBase::frameFrequenceStat(sendInfo * spBuf)
{
	  int  frameFrequenceStat[2]={0};
		u_int8_t sumCheck;
		u_int8_t  frameFrequence[4];
		frameFrequence[0]=0x21;
		frameFrequence[1]=frameFrequenceStat[0];
		frameFrequence[2]=frameFrequenceStat[1];
		sumCheck=sendCheck_sum(3, frameFrequence);
		frameFrequence[3]=(sumCheck&0xff);
		spBuf->byteSizeSend=spBuf->sendBuff[2]=0x07;
		memcpy(&(spBuf->sendBuff[3]),frameFrequence,4);

}

void CUserBase::vedioCompressStat(sendInfo * spBuf)
{
	  int  vedioCompressStat[2]={0};
		u_int8_t sumCheck;
		u_int8_t  vedioCompress[4];
		vedioCompress[0]=0x22;
		vedioCompress[1]=vedioCompressStat[0];
		vedioCompress[2]=vedioCompressStat[1];
		sumCheck=sendCheck_sum(3, vedioCompress);
		vedioCompress[3]=(sumCheck&0xff);
		spBuf->byteSizeSend=spBuf->sendBuff[2]=0x07;
		memcpy(&(spBuf->sendBuff[3]),vedioCompress,4);
}

void CUserBase::settingCmdRespon(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  settingCmdRespon[8];
	settingCmdRespon[0]=0x30;
	settingCmdRespon[1]=(u_int8_t) (mainProStat[ACK_config_Wblock]&0xff);
	settingCmdRespon[2]=(u_int8_t) (mainProStat[ACK_config_Wfield]&0xff);
	memcpy(&(settingCmdRespon[3]),&(Host_Ctrl[config_Wvalue]),4);
	sumCheck=sendCheck_sum(7, settingCmdRespon);
	settingCmdRespon[7]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x0b;
	memcpy(&(spBuf->sendBuff[3]),settingCmdRespon,8);
}

void CUserBase::readConfigSetting(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  readCfgSetting[8];
	readCfgSetting[0]=0x31;
	readCfgSetting[1]=(u_int8_t) (mainProStat[ACK_config_Rblock]&0xff);
	readCfgSetting[2]=(u_int8_t) (mainProStat[ACK_config_Rfield]&0xff);;
	memcpy(&(readCfgSetting[3]),&ACK_read,4);
	sumCheck=sendCheck_sum(7, readCfgSetting);
	readCfgSetting[7]=(sumCheck&0xff);
	spBuf->byteSizeSend=spBuf->sendBuff[2]=0x0b;
	memcpy(&(spBuf->sendBuff[3]),readCfgSetting,8);

}

void CUserBase::extExtraInputResponse(sendInfo * spBuf)
{

}


int   CUserBase::getSendInfo(int  respondId, sendInfo * psendBuf)
{
		switch(respondId){
		               case    ACK_selfTest:
		            	 //  startCheckAnswer(psendBuf);
		            	      break;
		               case   ACK_wordColor:
		                            break;
		               case   ACK_wordType:
		            	         break;
		               case  ACK_wordSize:
		            	         break;
		               case  NAK_wordColor:
		            	         break;
		               case   NAK_wordType:
		            	          break;
		               case  NAK_wordSize:
		            	          break;
		               case   ACK_mainVideoStatus:
		            	            mainVedioChannel(psendBuf);
		            	         break;
		               case   ACK_Channel_bindingStatus:
		            	          bindVedioChannel(psendBuf);
		            	          break;
		               case  ACK_avtTrkStatus:
		          	                 trackStatus(psendBuf);
		            	          break;
		               case  	ACK_avtTrkType:
		            	         trackTypes(psendBuf);
		            	          break;
		               case   ACK_avtErrorOutput:
		            	         trackErrOutput(psendBuf);
		            	          break;
		               case  ACK_mmtStatus:
		            	      mutilTargetNoticeStatus(psendBuf);
		            	         break;
		               case   ACK_mmtSelectStatus:
		            	        multilTargetNumSelectStatus(psendBuf);
		            	         break;
		               case   ACK_EnhStatus:
		            	        imageEnhanceStatus(psendBuf);
		            	          break;
		               case   ACK_MtdStatus:
		            	   moveTargetDetectedStat(psendBuf);
		            	          break;
		               case   ACK_TrkSearchStatus:
		            	        trackSearchStat(psendBuf);
		            	          break;
		               case   ACK_posMoveStatus:
		            	   trackFinetuningStat(psendBuf);
		            	          break;
		               case   ACK_moveAxisStatus:
		            	       confirmAxisStat(psendBuf);
		            	          break;
		               case   ACK_ElectronicZoomStatus:
		            	        ElectronicZoomStat(psendBuf);
		            	          break;
		               case  ACK_picpStatus:
		            	         pictureInPictureStat(psendBuf);
		            	          break;
		               case  	ACK_VideoChannelStatus:
		            	        vedioTransChannelStat(psendBuf);
		            	         break;
		               case  ACK_frameCtrlStatus:
		            	         break;
		               case  ACK_compression_quality:
		            	        break;
		               case   ACK_config_Write:
		            	   settingCmdRespon(psendBuf);
		            	        break;
		               case  ACK_config_Reab:
		            	         readConfigSetting(psendBuf);
		            	        break;
		               case  ACK_jos_Kboard:
		            	       extExtraInputResponse(psendBuf);
		            	         break;
		}
		return 0;
}


#if 0
u_int8_t  CUserBase::check_sum(u_int8_t  *pdata,uint len_t)
{
	u_int8_t cksum = 0;
	int n = len_t-1;

	for(; n>=0; n--)
	{
		cksum ^= *(pdata + n);
	}
	return  cksum;
}
#endif
  int CUserBase::check_InputDate(u_int8_t *pbuf, uint  len)
{
#if 0
	  u_int8_t recv_checksum = pbuf[len - 1];
	  u_int8_t  cal_checksum = check_sum(pbuf, len - 1);
	if (recv_checksum != cal_checksum){
        printf("ERR: checksum error,recv_checksum = %02x, cal_checksum = %02x\n", recv_checksum, cal_checksum);
        return -1;
	}
#endif
	return 0;
}

  int  CUserBase::process_ExtInputData( u_int8_t *data_buf, uint data_len)
  {
      if (-1 == check_InputDate(data_buf, data_len)){
  		return -1;
  	}
        processExtInput(data_buf,data_len);
        return 0;
  }


int   CUserBase:: processExtInput(u_int8_t *mExtInput,uint size)
{
		memcpy(&extCtrl,mExtInput,size);




	   return 0;
}



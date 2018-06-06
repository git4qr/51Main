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
CUserBase  *Sthis =NULL;
static u_int8_t  frame_head[]={0xEB, 0x53};
//static  pExtEventFunc  funcArray[]={NULL,track,SelSensor,TrkBoxCtrl,TrkSearch,upIris,downIris,focusUp,FocusDown,imgEnhance,autoIsriFocus,
															//	aimposx,aimposy,zoomlong, zoomshort, axisXctrl,axisYctrl };

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
					printf(" update cfg [%d] %d\r \n", i, cfg_blk_val[i]);
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
	if(EXT_Ctrl == NULL){
	EXT_Ctrl = new int[50];
	memset(EXT_Ctrl, 0, sizeof(int) * 50);
	}
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
	printf("INFO: startSelfCheak\r\n");
	int startCheck=rcvBufQue.at(4);
	Host_Ctrl[selfTest]=startCheck;
	EnableSelfTest();
}
void CUserBase:: mainVedioChannelSel()
{
	printf("INFO: mainVedioChannelSel\r\n");
	int mainVedioChannel=rcvBufQue.at(4);
	Host_Ctrl[mainVideo]=mainVedioChannel;
	SwitchSensor();
}

void CUserBase::channelBind()
{
	printf("INFO: channelBind\r\n");
	int channelBind=rcvBufQue.at(4);
	Host_Ctrl[Channel_binding]=channelBind;
}

void CUserBase::trackEnCmd()
{
	printf("INFO: track\r\n");
	int trackEnCmd= rcvBufQue.at(4);
	EXT_Ctrl[Cmd_Mesg_TrkCtrl-1]=trackEnCmd;
	EnableTrk();
}

void CUserBase::mutileTagartNotice()
{
    printf("INFO: enable mutile target!!\r\n");
    int mutiltargetNotice=rcvBufQue.at(4);
    EXT_Ctrl[Cmd_Mesg_Mmt-1]=mutiltargetNotice;
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
            	             TrkBoxCtrl();
            	             break;
         }
}

void   CUserBase::imageEnhance()
{
	  printf("INFO: image enhance \r\n");
	  int imageEnhance=rcvBufQue.at(4);
	  EXT_Ctrl[Cmd_Mesg_ImgEnh-1]=imageEnhance;
	  EnableIMG();
}

void CUserBase::trackFinetuning()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	         EXT_Ctrl[Cmd_Mesg_AIMPOS_X-1]=1;
            	             AIMPOS_X();
            	             break;
            case   0x2:
            	            EXT_Ctrl[Cmd_Mesg_AIMPOS_X-1]=2;
	                          AIMPOS_X();
            default:
            	            EXT_Ctrl[Cmd_Mesg_AIMPOS_X-1]=0;
         }
    switch(rcvBufQue.at(5) ){
    		case   0x1:
    						EXT_Ctrl[Cmd_Mesg_AIMPOS_Y-1]=1;;
    								AIMPOS_Y();
    									break;
    		case   0x2:
    						EXT_Ctrl[Cmd_Mesg_AIMPOS_Y-1]=2;
                    			AIMPOS_Y();
    		default:
    						EXT_Ctrl[Cmd_Mesg_AIMPOS_Y]=0;
    }
}

void CUserBase::confirmAxisInfo()
{
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
    			             Host_Ctrl[moveAxisX]=0;
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
	trakSearchx=  (rcvBufQue.at(5)<<8|rcvBufQue.at(6));
	trackSearchy=  (rcvBufQue.at(7)<<8|rcvBufQue.at(8));
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
            	EXT_Ctrl[Cmd_Mesg_IrisUp-1]=1;
            	IrisUp();
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
	printf("INFO:  Block=%d, filed=%d, value=%d\r\n",avtSetting.cmdBlock,avtSetting.cmdFiled,avtSetting.confitData);
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
	//printf("INFO: checksum %02x\r\n",checkSum);
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
                default:
                	        printf("INFO: Unknow  Cmd, please check!!!\r\n ");
                	 	 	ret =0;
                            break;
             }
	  }
	 rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+cmdLength);  //  analysis complete erase bytes
       return 1;
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



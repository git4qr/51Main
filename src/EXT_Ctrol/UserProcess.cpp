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
	Host_Ctrl = new int[30];
	memset(Host_Ctrl, 0, sizeof(int) * 30);
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
	int startCheck=rcvBufQue.at(4);
}
void CUserBase:: mainVedioChannelSel()
{
	int mainVedioChannel=rcvBufQue.at(4);

}

void CUserBase::channelBind()
{
	int channelBind=rcvBufQue.at(4);

}

void CUserBase::trackEnCmd()
{
	printf("INFO: track\r\n");
	int trackEnCmd= rcvBufQue.at(4);
	EnableTrk();
}

void CUserBase::mutileTagartNotice()
{
    printf("INFO: enable mutile target!!\r\n");
    int mutiltargetNotice=rcvBufQue.at(4);
	   EnableMmt();
}


void CUserBase::mutileTagartSelect()
{
	int mutileTargetSel=rcvBufQue.at(4);
}

void   CUserBase::imageEnhance()
{
	  printf("INFO: image enhance \r\n");
	  int imageEnhance=rcvBufQue.at(4);
	  EnableIMG();
}

void CUserBase::trackFineuning()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	         EXT_Ctrl[12]=1;
            	             AIMPOS_X();
            	             break;
            case   0x2:
            	            EXT_Ctrl[12]=1;
	                          AIMPOS_X();
            default:
            	            EXT_Ctrl[12]=0;
         }
    switch(rcvBufQue.at(5) ){
    		case   0x1:
    						EXT_Ctrl[13]=0;
    								AIMPOS_Y();
    									break;
    		case   0x2:
    						EXT_Ctrl[13]=0;
                    			AIMPOS_Y();
    		default:
    						EXT_Ctrl[13]=0;
    }
}

void CUserBase::confirmAxisInfo()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	             ExtInputCtrl[0]=0;
            	             AIMPOS_X();
            	             break;
            case   0x2:
	                        ExtInputCtrl[0]=0;
	                          AIMPOS_X();
            default:
                        	ExtInputCtrl[0]=0;
         }
    switch(rcvBufQue.at(5) ){
    		case   0x1:
    							ExtInputCtrl[0]=0;
    								AIMPOS_Y();
    									break;
    		case   0x2:
                    		ExtInputCtrl[0]=0;
                    			AIMPOS_Y();
    		default:
                			ExtInputCtrl[0]=0;
    }
}

void CUserBase::ElectronicZoom()
{
    switch(rcvBufQue.at(4) ){
            case   0x1:
            	             ExtInputCtrl[0]=0;
            	             AIMPOS_X();
            	             break;
            case   0x2:
	                        ExtInputCtrl[0]=0;
	                          AIMPOS_X();
	                          break;
            case   0x3:
                        	ExtInputCtrl[0]=0;
                        	break;
            case   0x4:
            	            break;
         }
}

void CUserBase::trackSearch()
{
	int trakSearchx,trackSearchy;
	trakSearchx=trackSearchy=0;
	trakSearchx=  (rcvBufQue.at(5)<<8|rcvBufQue.at(6));
	trackSearchy=  (rcvBufQue.at(7)<<8|rcvBufQue.at(8));
	   switch(rcvBufQue.at(4)){
	  	  	  	  	  case  0x01:
	  	  	  	  		  	  	  	//  EXT_Ctrl[11]=1;
	  	  	  	  		              break;
	  	  	  	  	  case  0x02:
	  	  	  	  		  	  	  	 // EXT_Ctrl[11]=0;
	  	  	  	  		              break;
	       }
}

void CUserBase::confirmAxisSave()
{

}

void CUserBase::moveTargetDetected()
{
	   switch(rcvBufQue.at(4)){
	  	  	  	  	  case  0x01:
	  	  	  	  		  	  	  	//  EXT_Ctrl[11]=1;
	  	  	  	  		              break;
	  	  	  	  	  case  0x02:
	  	  	  	  		  	  	  	 // EXT_Ctrl[11]=0;
	  	  	  	  		              break;
	       }
}
void CUserBase::pictrueInPicture()
{
	 int channelNum=rcvBufQue.at(5);
	   switch(rcvBufQue.at(4)){
	  	  	  	  	  case  0x01:
	  	  	  	  		  	  	  	//  EXT_Ctrl[11]=1;
	  	  	  	  		              break;
	  	  	  	  	  case  0x02:
	  	  	  	  		  	  	  	 // EXT_Ctrl[11]=0;
	  	  	  	  		              break;
	       }
}

void CUserBase::selectVedisTransChannel()
{
     int  selTransChannel=rcvBufQue.at(4);
}

void CUserBase::frameFrequenceCtrl()
{

}

void CUserBase::vedioCompressQuality()
{

}
void CUserBase::opticalZoomCtrl()
{

}

void  CUserBase::irisCtrl()
{

}

void CUserBase::foucsCtrl()
{

}
void CUserBase::fontColor()
{

}
void CUserBase::fontStyle()
{

}
void CUserBase::fontSize()
{

}
void  CUserBase::fontDisplayCtrl()
{

}
void  CUserBase::configSetting()
{
	uint8_t  tempbuf[4];
	memset(&avtSetting,0,sizeof(avtSetting));
	avtSetting.cmdBlock=rcvBufQue.at(4);
	avtSetting.cmdFiled=rcvBufQue.at(5);
	for(int m=6;m<10;m++)
		tempbuf[m-6]=rcvBufQue.at(m);
	memcpy(&avtSetting.confitData,tempbuf,sizeof(int));
	printf("INFO:  Block=%d, filed=%d, value=%d\r\n",avtSetting.cmdBlock,avtSetting.cmdFiled,avtSetting.confitData);
	Host_Ctrl[config_Wblock]=avtSetting.cmdBlock;
    Host_Ctrl[config_Wfield]=avtSetting.cmdFiled;
    Host_Ctrl[config_Wvalue]=avtSetting.confitData;
	Enableconfig();
}

void CUserBase::readCurrentSetting()
{

}

void   CUserBase::extExtraInputCtrl()
{

}

int  CUserBase::prcRcvFrameBufQue()
{
    int ret =  -1;
    printf("INFO: prcocee Buff queue!!\r\n");
	 //  for(int i=0;i<rcvBufQue.size();i++)
	//   {
	//	   printf("%02x-",rcvBufQue.at(i));

	 //  }
	int  cmdLength= rcvBufQue.at(2);
//	printf("INFO: length(%d)\r\n",cmdLength);
	if(cmdLength<5) {
		printf("Worning::  Invaild frame\r\n");
		 rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+cmdLength);  //  analysis complete erase bytes
		return ret;
	}
	u_int8_t checkSum = check_sum(cmdLength);
	printf("INFO: checksum %02x\r\n",checkSum);
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
                	                        trackFineuning();
                            break;
                case    0x09:
                							confirmAxisInfo();
                	       break;
                case    0x0a:
                	                       ElectronicZoom();
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



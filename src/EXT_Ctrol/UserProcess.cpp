#include <string.h>
#include <algorithm>
#include <vector>
#include <map>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "UserProcess.h"
#include "ExtFunc.h"
#include "finder.h"

using namespace cv;
using namespace std;

#define   btnsettingcfgfilesize   (10)
#define  KeyMAXSize  (12)
#define   MAX_CFG_ID  (19)
static int  cfg_blk_val[16];
CUserBase  *Sthis =NULL;
static u_int8_t  frame_head[]={0xEB, 0x53};
static  pExtEventFunc  funcArray[]={NULL,track,SelSensor,TrkBoxCtrl,TrkSearch,upIris,downIris,focusUp,FocusDown,imgEnhance,autoIsriFocus,
																aimposx,aimposy,zoomlong, zoomshort, axisXctrl,axisYctrl };

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
}

CUserBase::~CUserBase()
{
	Destroy();
}

void CUserBase::PreInit()
{
	EXT_Ctrl = new int[35];
	memset(EXT_Ctrl, 0, sizeof(int) * 35);
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

int CUserBase::startSelfCheak()
{

}
int CUserBase:: mainVedioChannelSel()
{

}

int CUserBase::channelBind()
{

}

int CUserBase::trackEnCmd()
{
	printf("INFO: track\r\n");
  if(0x1==rcvBufQue.at(4)){
	  ExtInputCtrl[0]=0;
  }else  ExtInputCtrl[0]=1;
	EnableTrk();
	return 1;
}

int CUserBase::mutileTagartNotice()
{
	  if(0x1==rcvBufQue.at(4)){
		  ExtInputCtrl[0]=0;
	  }else  ExtInputCtrl[0]=1;
		EnableTrk();
		return 1;

}


int CUserBase::mutileTagartSelect()
{
      switch(rcvBufQue.at(4)){
      	  	  	  	  	  case  0x01:
      	  	  	  	  		              break;
      	  	  	  	  	  case  0x02:
      	  	  	  	  		              break;
      	  	  	  	  	  case  0x03:
      	  	  	  	  		               break;
      	  	  	  	  	  case  0x04:
      	  	  	  	  		              break;
      	  	  	  	  	  case  0x05:
      	  	  	  	  		               break;
      }
	return 1;
}

int   CUserBase::imageEnhance()
{
	  if(0x1==rcvBufQue.at(4)){
		  ExtInputCtrl[0]=0;
	  }else  ExtInputCtrl[0]=1;
	  EnableIMG();
		return 1;
}

int CUserBase::trackFineuning()
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
		return 1;
}

int CUserBase::confirmAxisInfo()
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
		return 1;
}

int CUserBase::ElectronicZoom()
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

int  CUserBase::AVTsetting()
{
	avtSetting.cmdBlock=rcvBufQue.at(4) ;
	avtSetting.cmdFiled=rcvBufQue.at(5) ;
	avtSetting.confitData=(int)(rcvBufQue.at(6)| rcvBufQue.at(7)<<8| rcvBufQue.at(8)<<16| rcvBufQue.at(9)<<24);
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
                							ret=startSelfCheak();
                	       break;
                case  0x02:
                	                       ret =mainVedioChannelSel();
                            break;
                case    0x03:
                							ret=channelBind();
                	       break;
                case  0x04:
                							ret=trackEnCmd();
                            break;
                case    0x05:
                							ret=mutileTagartNotice();
                	       break;
                case  0x06:
                	                        ret=mutileTagartSelect();
                            break;
                case    0x07:
                	                        ret=imageEnhance();
                	       break;
                case  0x08:
                	                        ret=trackFineuning();
                            break;
                case    0x09:
                							ret=confirmAxisInfo();
                	       break;
                case    0x0a:
                	                       ret=ElectronicZoom();
                	       break;
                case  0x0b:
                            break;
                case    0x0c:
                	       break;
                case  0x0d:
                            break;
                case    0x0e:
                	       break;
                case    0x0f:
                            break;
                case    0x10:
                	       break;
                case  0x11:
                            break;
                case    0x12:
                            break;
                case    0x13:
                	       break;
                case   0x14:
                            break;
                case    0x20:
                	       break;
                case  0x21:
                            break;
                case    0x22:
                	       break;
                case  0x24:
                            break;
                case    0x30:
                	                 ret=AVTsetting();
                	       break;
                case  0x31:
                            break;
                default:
                	        printf("INFO: Unknow  Cmd, please check!!!\r\n ");
                	 	 	ret =0;
                            break;
       }

	}
	 rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+cmdLength);  //  analysis complete erase bytes
       return ret;
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



#include <termios.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include "UartProcess.h"
#include "osa_sem.h"
extern OSA_SemHndl  m_semHndl;
static void printfUARTRCVbuff(char *mBuf,int size);
static u_int8_t  frame_head[]={0xEB, 0x53};
CUartProcess::CUartProcess()
{
	m_UartRun=false;
	m_port=-1;
}

CUartProcess::~CUartProcess()
{
	m_UartRun = false;
	OSA_thrJoin(&uartGetDataThrID);
	OSA_thrJoin(&uartSendDataThrID);
}
int CUartProcess::open_Uart(const int nport)
{
  char path_name[20];
   sprintf(path_name, "/dev/ttyTHS%d", nport);
    if((m_port = open(path_name, O_RDWR| O_NOCTTY |O_NDELAY)) <= 0)
  {
    printf("ERR: Can not open the Uart port --(%s)\r\n",path_name);
       exit(-1);
  }
   if(!isatty(m_port))
   {
   }
   return 0;
}
int CUartProcess::set_Uart(const int baud_rate, const int data_bits, char parity, const int stop_bits)
{
    struct termios newtio;
    memset(&newtio,  0, sizeof(newtio));

    if(tcgetattr(m_port, &newtio))
    {
        printf( "tcgetattr error : %s\n", strerror(errno));
        return -1;
    }
    //set baud rate
    switch (baud_rate)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;

    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
    }

    //set data bits
    newtio.c_cflag &= ~CSIZE;
    switch(data_bits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio .c_cflag |= CS8;
        break;
    default:
        newtio.c_cflag |= CS8;
        break;
    }
    //set stop bits
    switch (stop_bits)
    {
    default:
    		case 1:
    		{
    			newtio.c_cflag &= ~CSTOPB;
    		}
    		break;
    		case 2:
    		{
    			newtio.c_cflag |= CSTOPB;
    		}
    		break;
    }
    //set parity
    switch (parity)
    {
	default:
	case 'n':
	case 'N':
	{
		newtio.c_cflag &= ~PARENB;
		newtio.c_iflag &= ~INPCK;
	}
	break;

	case 'o':
	case 'O':
	{
		newtio.c_cflag |= (PARODD|PARENB);
		newtio.c_iflag |= INPCK;
	}
	break;

	case 'e':
	case 'E':
	{
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		newtio.c_cflag |= INPCK;
	}
	break;

	case 's':
	case 'S':
	{
		newtio.c_cflag &= ~PARENB;
		newtio.c_cflag &= ~CSTOPB;
		newtio.c_cflag |= INPCK;
	}
	break;
    }

    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    newtio.c_oflag &= ~OPOST;                 // 杈撳嚭鏁版嵁妯″紡锛屽師濮嬫暟鎹?    Opt.c_oflag &= ~(ONLCR | OCRNL);          //娣诲姞鐨?
    newtio.c_iflag &= ~(ICRNL | INLCR);
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);   // 涓嶄娇鐢ㄨ蒋浠舵祦鎺э紱
    newtio.c_cflag &= ~CRTSCTS;   //  涓嶄娇鐢ㄦ祦鎺у埗

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 1;
    tcflush(m_port, TCIOFLUSH);
    if(tcsetattr(m_port, TCSANOW, &newtio)){
        return -1;
    }
    printf( "tcsetattr done, baud_rate:%d, data_bits:%d, parity:%c, stop_bit:%d\n", baud_rate, data_bits, parity, stop_bits);
    return 0;
}

int  CUartProcess::Create()
{
		  int iret;

		 open_Uart(1);
		 set_Uart(115200, 8, 'E', 1);
			m_UartRun=true;
			iret =   OSA_thrCreate(&uartGetDataThrID,
												uartGetDataThrFun,
																0,
																0,
														(void*)this);

			iret =   OSA_thrCreate(&uartSendDataThrID,
												uartSendDataThrFun,
																0,
																0,
														(void*)this);
	return 0;

}

int  CUartProcess::UartRecv(void *rcv_buf,int data_len)
{
		int fs_sel,len;
		fd_set fd_uartRead;
		struct timeval timeout;
		FD_ZERO(&fd_uartRead);
		FD_SET(m_port,&fd_uartRead);

	    timeout.tv_sec = 0;
		timeout.tv_usec = 50000;

		fs_sel = select(m_port+1,&fd_uartRead,NULL,NULL,&timeout);
		if(fs_sel){
			len = read(m_port,rcv_buf,data_len);
			return len;
		}
		else if(-1 == fs_sel){
			printf("ERR: Uart Recv  select  Error!!\r\n");
			return -1;
		}
		else if(0 == fs_sel){
			//printf("Warning: Uart Recv  time  out!!\r\n");
			return 0;
		}
}

/*
 *flags description:
 *udata_buf: data read from serial port
 *size: length of udata_buf
 */
int CUartProcess::get_uartdata_process(unsigned char *uartdata, int size)
{
    int uartdata_pos = 0;
    int ret = -1;
    while (uartdata_pos< size)
    {
        if((0 == swap_data.reading) || (2 == swap_data.reading))
        {
            if(frame_head[swap_data.len] == uartdata[uartdata_pos])
            {
                swap_data.buf[swap_data.pos++] =  uartdata[uartdata_pos++];
                swap_data.len++;
                swap_data.reading = 2;
                if(swap_data.len == (sizeof(frame_head)/sizeof(u_int8_t)))
                    swap_data.reading = 1;
            }
            else
            {
                uartdata_pos++;
                if(2 == swap_data.reading)
                    memset(&swap_data, 0, sizeof(struct data_buf));
            }
    	}
        else if(1 == swap_data.reading)
        {
            swap_data.buf[swap_data.pos++] = uartdata[uartdata_pos++];
			swap_data.len++;
			if (swap_data.len == RECV_FRAME_LEN)
			{
                ret= process_ExtInputData(swap_data.buf, swap_data.len);
				memset(&swap_data, 0, sizeof(struct data_buf));
			}
        }
    }
    return ret;
}

void CUartProcess::prorcvbufdata(uint8_t *inBuf,int inCnt,uint8_t *outBuf,int *destCnt,int *readStatus)
{
	    int i;
	    for(i=0; i<inCnt; i++)
	    {
	        if(inBuf[i] == 0xEB && inBuf[i+2] == 0x53 && inBuf[i+3] == 0x00)//header
	        {
	            outBuf[(*destCnt)++] = inBuf[i];
	            *readStatus = 1;
	            //print_frame("header",dest,dest_cnt);
	            continue;
	        }
	        if(*readStatus == 1)//body
	        {
	            outBuf[(*destCnt)++] = inBuf[i];
	            //print_frame("body",dest,dest_cnt);
	        }
	        if(*destCnt == outBuf[1])//tail
	        {
	          //  print_frame("tail",outBuf,*destCnt);
	            *readStatus = 0;
	            *destCnt = 0;
	            memset(outBuf,-1,sizeof(outBuf));
	            memset(inBuf,0,sizeof(inBuf));
	            continue;
	        }
	    }
}

#if 0
int  CUartProcess::UartRecvDataThread()
{
	int sizeRcv;
 uint8_t dest[1024]={0};
	int read_status = 0;
	int dest_cnt = 0;
	unsigned char  tmpRcvBuff[RECV_BUF_SIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));
	while(m_UartRun){
		sizeRcv= UartRecv(tmpRcvBuff,RECV_BUF_SIZE);
		    if(sizeRcv<=0) continue;
		   else{
			   	  printf("INFO: Uart rcv data size(%d)!!\r\n",sizeRcv);
			    get_uartdata_process(tmpRcvBuff,sizeRcv);
				  // prorcvbufdata(tmpRcvBuff,sizeRcv,dest,&dest_cnt,&read_status);
		    }
	  }
	return 0;
}
#endif

int  CUartProcess::UartRecvDataThread()
{
	int sizeRcv;
 uint8_t dest[1024]={0};
	int read_status = 0;
	int dest_cnt = 0;
	unsigned char  tmpRcvBuff[RECV_BUF_SIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));
	while(m_UartRun){
		sizeRcv= UartRecv(tmpRcvBuff,RECV_BUF_SIZE);
		if(sizeRcv>0){
			    printf("INFO: Uart receive data: \r\n");
			   for(int i=0;i<sizeRcv;i++)
			   {
				   printf("%02x ",tmpRcvBuff[i]);
				   rcvBufQue.push_back(tmpRcvBuff[i]);
			   }
			  printf("\n");
			   memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));
		}
		   if(rcvBufQue.size()<5)   continue;    //less the smallest  frame   and then  receive the data
           int off_t=findFrameHeader();     //find  frame header
            if(off_t<0){
            	 rcvBufQue.clear();
            	 continue;
			}
             if(off_t)
             rcvBufQue.erase(rcvBufQue.begin(),rcvBufQue.begin()+off_t);  // erase  unused bytes
            	//now vector start 0xEB  0x53
             if(rcvBufQue.size()<5)    continue; //less the smallest  frame   and then continue  receive the data
              if( rcvBufQue.at(2)>rcvBufQue.size())   continue;  // judge the  length  byte  upper the  vector byte size
             prcRcvFrameBufQue();
	}
	return 0;
}


#if 1
int  CUartProcess::UartSendDataThread()
{
	    int mRespCmd;
	    int n;
	    int result,retVle;
	    struct timeval    sendTimeout;
		while(m_UartRun){
				OSA_semWait( & m_semHndl,OSA_TIMEOUT_FOREVER);
				 memcpy(&repSendBuffer.sendBuff[0],frame_head,sizeof(frame_head));
			     getSendInfo(feedback, &repSendBuffer);
		       //  for(n=0;n<repSendBuffer.byteSizeSend;n++)
			    //          printf("%02x ",repSendBuffer.sendBuff[n]);
		         //    printf("\n");
			        retVle = write (m_port, &repSendBuffer.sendBuff,repSendBuffer.byteSizeSend);
		  }
		return 0;
}

#else if
char *Uarttest="asdfghjkl";
int  CUartProcess::UartSendDataThread()
{
	    int result,retVle;
	    struct timeval    sendTimeout;
		while(m_UartRun){
		//	OSA_semWait( semhandl,OSA_TIMEOUT_FOREVER);
			sendTimeout.tv_sec   = 3;
			sendTimeout.tv_usec = 33000;
			 result = select(0,NULL, NULL, NULL, &sendTimeout);
			if(result <0){
				printf("ERR: Connect --select Send error\r\n");
			            break;
			}
		  if(result == 0){
			  //test send
			 printf("INFO: time out\r\n");
			 printf("INFO: the size: %d\r\n",strlen(Uarttest));
		     retVle = write (m_port, Uarttest,strlen(Uarttest));
		//  printf("%s, %d, retVle=%d, my port write:%02x\n",__FILE__, __LINE__, retVle,Uarttest[0]);
			 if(retVle<=0){
			 	  	  printf("ERR: Uart send error!!\r\n");
			     break;
			    }else
			   	continue;
		      }
		  }
		return 0;
}
#endif
static void printfUARTRCVbuff(char *mBuf,int size)
{
	for(int i=0;i<size;i++)
		printf("INFO: RCVbuf ID: %d---Value: %c \r\n ",i,mBuf[i]);
}

int CUartProcess::UartFlush(int fd)
{
	if(tcflush(fd,TCIOFLUSH) == -1)
	{
		return -1;
	}

	return 0;
}

int CUartProcess::UartFlushWrite(int fd)
{
	if(tcflush(fd,TCOFLUSH) == -1)
	{
		return -1;
	}

	return 0;
}

int CUartProcess::UartFlushRead(int fd)
{
	if(tcflush(fd,TCIFLUSH) == -1)
	{
		return -1;
	}

	return 0;
}


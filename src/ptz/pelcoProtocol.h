#ifndef _PELCO_PROTOCOL_H
#define _PELCO_PROTOCOL_H

//#ifndef _BASETSD_H_
typedef int				INT32, *PINT32;
typedef short			INT16, *PINT16;
//typedef char			INT8, *PINT8;
typedef unsigned int	UINT32, *PUINT32;
typedef unsigned short	UINT16, *PUINT16;
typedef unsigned char   UINT8, *PUINT8;
typedef long			LONG, *PLONG;
typedef unsigned long   ULONG, *PULONG;
//typedef int             BOOL
//#endif

#ifndef BOOL
#define BOOL	int
#define TRUE	1
#define FALSE	0
#endif

enum{
	PTZ_MOVE_Stop = 0,PTZ_MOVE_Up,PTZ_MOVE_Down,PTZ_MOVE_Left, PTZ_MOVE_Right
};


//////////////////////////////////////////////////////////////////////////
//  pelco_d 
#define PELCO_D_SYNC             0xFF
#define PELCO_D_SPEED_PEAKVALUE  0x3F
//  pelco_p
#define PELCO_P_STX              0xA0
#define PELCO_P_ETX              0xAF

#define PELCO_PRESET_SCAN_AUTO			99	//Begin Auto-scan
#define PELCO_PRESET_SCAN_FRAME			98	//Begin Frame Scan
#define PELCO_PRESET_SCAN_RANDOM			97	//Begin Random Scan
#define PELCO_PRESET_SCAN_STOP			96	//Stop Scan
#define PELCO_PRESET_ENTERMENU			95	//Enter Menu Mode
#define PELCO_PRESET_REMOTERESET			94	//Remote Reset
#define PELCO_PRESET_RIGHTLIMIT			93	//Set Right Limit Stop
#define PELCO_PRESET_LEFTLIMIT			92	//Set Left Limit Stop
#define PELCO_PRESET_ZEROPAN				34	//Home (return to 0 pan position)
#define PELCO_PRESET_FLIP					33	//Flip

enum{
	PTZ_PRESET_SET = 3, PTZ_PRESET_CLEAR = 5, PTZ_PRESET_GOTO = 7
};

enum{
	PTZ_Pattern_SetStart = 31, PTZ_Pattern_SetStop=33, PTZ_Pattern_Run = 35
};

enum{
	PTZ_QUERY_Pan = 0x51, 
	PTZ_QUERY_Tilt = 0x53, 
	PTZ_QUERY_Zoom = 0x55,
	PTZ_QUERY_Magnification = 0x61, 
	PTZ_QUERY_DeviceType = 0x6B,
	PTZ_QUERY_DiagnosticInfo = 0x6F
};

#define PELCO_EXTCOMMAND_SetAux			0x09
#define PELCO_EXTCOMMAND_ClearAux		0x0B
#define PELCO_EXTCOMMAND_RemoteReset	0x0F
#define PELCO_EXTCOMMAND_ZoneStart		0x11
#define PELCO_EXTCOMMAND_ZoneEnd		0x13
#define PELCO_EXTCOMMAND_Writechar		0x15
#define PELCO_EXTCOMMAND_ClearScreen	0x17
#define PELCO_EXTCOMMAND_AlarmAck		0x19
#define PELCO_EXTCOMMAND_ZoneScanOn		0x1B
#define PELCO_EXTCOMMAND_ZoneScanOff	0x1D
#define PELCO_EXTCOMMAND_PatternStart	0x1F
#define PELCO_EXTCOMMAND_PatternStop	0x21
#define PELCO_EXTCOMMAND_PatternRun		0x23

#pragma pack(1)

typedef struct {
	UINT8 bySynchByte;
	UINT8 byAddress;
	union{
		UINT8 byCmd1;
		struct{
			UINT8 FocusNear : 1;  //��������
			UINT8 IrisOpen  : 1;  //��Ȧ��
			UINT8 IrisClose : 1;  //��Ȧ�ر�
			UINT8 CameraOn  : 1;
			UINT8 AutoScan  : 1;
			UINT8 Reserved2 : 1;
			UINT8 Reserved1 : 1;
			UINT8 Sense     : 1;
		}tCmd1;
	};
	union{
		UINT8 byCmd2;
		struct{
			UINT8 Reserved  : 1;
			UINT8 Right     : 1;
			UINT8 Left      : 1;
			UINT8 Up        : 1;
			UINT8 Down      : 1;
			UINT8 ZoomTele  : 1;  //�ӽǱ�խ
			UINT8 ZoomWide  : 1;  //�ӽǱ��
			UINT8 FocusFar  : 1;  //������Զ
		}tCmd2;
	};
	UINT8 byData1;
	UINT8 byData2;
	UINT8 byCheckSum;
}PELCO_D_REQPKT, *LPPELCO_D_REQPKT;

typedef struct {
	UINT8 bySynchByte;
	UINT8 byAddress;
	union{
		UINT8 byValue;
		struct{
			UINT8 Alarm8  : 1;
			UINT8 Alarm7  : 1;
			UINT8 Alarm6  : 1;
			UINT8 Alarm5  : 1;
			UINT8 Alarm4  : 1;
			UINT8 Alarm3  : 1;
			UINT8 Alarm2  : 1;
			UINT8 Alarm1  : 1;
		}tValue;
	};
	UINT8 byCheckSum;
}PELCO_D_RESPPKT, *LPPELCO_D_RESPPKT;


typedef struct {
	UINT8 bySTX;
	UINT8 byAddress;
	union{
		UINT8 byCmd1;
		struct{
			UINT8 FocusFar  : 1;  //������Զ
			UINT8 FocusNear : 1;  //��������
			UINT8 IrisOpen  : 1;  //��Ȧ��
			UINT8 IrisClose : 1;  //��Ȧ�ر�
			UINT8 CameraOn  : 1;
			UINT8 AutoScan  : 1;
			UINT8 Camera    : 1;
			UINT8 Reserved  : 1;
		}tCmd1;
	};
	union{
		UINT8 byCmd2;
		struct{
			UINT8 PanTilt   : 1;
			UINT8 Right     : 1;
			UINT8 Left      : 1;
			UINT8 Up        : 1;
			UINT8 Down      : 1;
			UINT8 ZoomTele  : 1;  //�ӽǱ�խ
			UINT8 ZoomWide  : 1;  //�ӽǱ��
			UINT8 Reserved  : 1;
		}tCmd2;
	};
	UINT8 byData1;
	UINT8 byData2;
	UINT8 byETX;
	UINT8 byCheckSum;
}PELCO_P_REQPKT, *LPPELCO_P_REQPKT;
#pragma pack(4)


class IPelcoDFormat
{
public:
	IPelcoDFormat(){}
	virtual ~IPelcoDFormat(){}
	static void MakeSumCheck(LPPELCO_D_REQPKT pPkt);
	static void PktFormat(LPPELCO_D_REQPKT pPkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress = 0);
	//
	//Receiver 10, Camera on, Focus far, Left, turbo speed 
	//								FF, 0A, 88, 90, 00, 40, 62
	static void MakeCameraOn(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeCameraOff(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeMove(LPPELCO_D_REQPKT pPkt, INT32 iDirection, UINT8 bySpeed = 0x20,
		BOOL bClear = TRUE, UINT8 byAddress = 0);
	static void MakeFocusFar(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeFocusNear(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeFocusStop(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeIrisOpen(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeIrisClose(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeZoomWide(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeZoomTele(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeZoomStop(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeAutoScan(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);

	// EXTENDED COMMANDS
	static void MakePresetCtrl(LPPELCO_D_REQPKT pPkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0);
	static void MakePatternCtrl(LPPELCO_D_REQPKT pPkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0);
	static void MakeSetZoomSpeed(LPPELCO_D_REQPKT pPkt, UINT8 byValue, UINT8 byAddress = 0);
	static void MakeSetFocusSpeed(LPPELCO_D_REQPKT pPkt, UINT8 byValue, UINT8 byAddress = 0);
	static void MakeRemoteReset(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeExtCommand(LPPELCO_D_REQPKT pPkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt = 0, UINT8 byAddress = 0);
	static void MakeDummy(LPPELCO_D_REQPKT pPkt, UINT8 byAddress = 0);
};


class IPelcoPFormat
{
public:
	IPelcoPFormat(){}
	virtual ~IPelcoPFormat(){}
	static void MakeSumCheck(LPPELCO_P_REQPKT pPkt);
	static void PktFormat(LPPELCO_P_REQPKT pPkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress = 0);
	//
	static void MakeCameraOn(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeCameraOff(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeMove(LPPELCO_P_REQPKT pPkt, INT32 iDirection, UINT8 bySpeed = 0x20,
		BOOL bClear = TRUE, UINT8 byAddress = 0);
	static void MakeFocusFar(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeFocusNear(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeFocusStop(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeIrisOpen(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeIrisClose(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeZoomWide(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeZoomTele(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeZoomStop(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeAutoScan(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);

	// EXTENDED COMMANDS
	static void MakePresetCtrl(LPPELCO_P_REQPKT pPkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0);
	static void MakeSetZoomSpeed(LPPELCO_P_REQPKT pPkt, UINT8 byValue, UINT8 byAddress = 0);
	static void MakeSetFocusSpeed(LPPELCO_P_REQPKT pPkt, UINT8 byValue, UINT8 byAddress = 0);
	static void MakeRemoteReset(LPPELCO_P_REQPKT pPkt, UINT8 byAddress = 0);
	static void MakeExtCommand(LPPELCO_P_REQPKT pPkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt = 0, UINT8 byAddress = 0);
};

#endif

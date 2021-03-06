#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "osa.h"
#include "pelcoProtocol.h"

char strPtzProtocol[][16] = {
	"Pelco_D","Pelco_P",
};

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

void IPelcoDFormat::MakeSumCheck(LPPELCO_D_REQPKT pPkt)
{
	if(pPkt == NULL)
		return ;
	UINT8 *pBuf = (UINT8 *)pPkt;
	pPkt->bySynchByte = PELCO_D_SYNC;
	pPkt->byCheckSum = 0;
	pPkt->byCheckSum += pBuf[1];
	pPkt->byCheckSum += pBuf[2];
	pPkt->byCheckSum += pBuf[3];
	pPkt->byCheckSum += pBuf[4];
	pPkt->byCheckSum += pBuf[5];
	//for(int i = 1; i < sizeof(PELCO_D_REQPKT) - 1 ; i++){
	//	pPkt->byCheckSum += pBuf[i];
		//pPkt->byCheckSum ^= pBuf[i];
	//}
}

void IPelcoDFormat::PktFormat(LPPELCO_D_REQPKT pPkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = cmd1;
	pPkt->byCmd2 = cmd2;
	pPkt->byData1 = data1;
	pPkt->byData2 = data2;
	MakeSumCheck(pPkt);
}

void IPelcoDFormat::MakeCameraOn(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x88;
	pPkt->byCmd2 = 0;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}

void IPelcoDFormat::MakeCameraOff(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x08;
	pPkt->byCmd2 = 0;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeMove(LPPELCO_D_REQPKT pPkt, INT32 iDirection, UINT8 bySpeed /* = 0x20 */, BOOL bClear /* = TRUE */, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	if(bClear){
		pPkt->byCmd2 = 0;
		pPkt->byData1 = 0;
		pPkt->byData2 = 0;
	}
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}

	switch(iDirection)
	{
	case PTZ_MOVE_Stop:
		pPkt->byCmd2 = 0;
		pPkt->byData1 = 0;
		pPkt->byData2 = 0;
		break;
	case PTZ_MOVE_Up:
		pPkt->tCmd2.Up = 1;
		pPkt->tCmd2.Down = 0;
		pPkt->byData2 = bySpeed;
		break;
	case PTZ_MOVE_Down:
		pPkt->tCmd2.Up = 0;
		pPkt->tCmd2.Down = 1;
		pPkt->byData2 = bySpeed;
		break;
	case PTZ_MOVE_Left:
		pPkt->tCmd2.Left = 1;
		pPkt->tCmd2.Right = 0;
		pPkt->byData1 = bySpeed;
		break;
	case PTZ_MOVE_Right:
		pPkt->tCmd2.Left = 0;
		pPkt->tCmd2.Right = 1;
		pPkt->byData1 = bySpeed;
		break;
	}
	MakeSumCheck(pPkt);
}

void IPelcoDFormat::MakeFocusFar(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd1.FocusNear = 0;
	pPkt->tCmd2.FocusFar = 1;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeFocusNear(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd1.FocusNear = 1;
	pPkt->tCmd2.FocusFar = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeFocusStop(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->tCmd1.FocusNear = 0;
	pPkt->tCmd2.FocusFar = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeIrisOpen(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.IrisOpen = 1;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeIrisClose(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.IrisClose = 1;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeZoomWide(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd2.ZoomWide = 1;
	pPkt->tCmd2.ZoomTele = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeZoomTele(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd2.ZoomWide = 0;
	pPkt->tCmd2.ZoomTele = 1;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeZoomStop(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->tCmd2.ZoomWide = 0;
	pPkt->tCmd2.ZoomTele = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeAutoScan(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.AutoScan = 1;
	pPkt->byCmd2 = 0;
	MakeSumCheck(pPkt);
}

//////////////////////////////////////////////////////////////////////////
//Set Preset 00 03 00 01 to 20
void IPelcoDFormat::MakePresetCtrl(LPPELCO_D_REQPKT pPkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = nCtrlType;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
//////////////////////////////////////////////////////////////////////////
//Set Pattern 00 03 00 01 to 20
void IPelcoDFormat::MakePatternCtrl(LPPELCO_D_REQPKT pPkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = nCtrlType;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
//Remote Reset 00 0F 00 00
void IPelcoDFormat::MakeRemoteReset(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = 0x0F;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}
//Set Zoom Speed 00 25 00 00 to 03
void IPelcoDFormat::MakeSetZoomSpeed(LPPELCO_D_REQPKT pPkt, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = 0x25;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
//Set Focus Speed 00 27 00 00 to 03
void IPelcoDFormat::MakeSetFocusSpeed(LPPELCO_D_REQPKT pPkt, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = 0x27;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeExtCommand(LPPELCO_D_REQPKT pPkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt /* = 0 */, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = nCmdType;
	pPkt->byData1 = byDataExt;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeDummy(LPPELCO_D_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = 0x0D;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}
//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

void IPelcoPFormat::MakeSumCheck(LPPELCO_P_REQPKT pPkt)
{
	if(pPkt == NULL)
		return ;
	UINT8 *pBuf = (UINT8 *)pPkt;
	pPkt->bySTX = PELCO_P_STX;
	pPkt->byETX = PELCO_P_ETX;
	pPkt->byCheckSum = 0;
	for(int i = 0; i < sizeof(PELCO_P_REQPKT) - 1 ; i++){
		pPkt->byCheckSum ^= pBuf[i];
	}
}

void IPelcoPFormat::PktFormat(LPPELCO_P_REQPKT pPkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = cmd1;
	pPkt->byCmd2 = cmd2;
	pPkt->byData1 = data1;
	pPkt->byData2 = data2;
	MakeSumCheck(pPkt);
}

void IPelcoPFormat::MakeCameraOn(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x88;
	pPkt->byCmd2 = 0;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}

void IPelcoPFormat::MakeCameraOff(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x08;
	pPkt->byCmd2 = 0;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}


void IPelcoPFormat::MakeMove(LPPELCO_P_REQPKT pPkt, INT32 iDirection, UINT8 bySpeed /* = 0x20 */, BOOL bClear /* = TRUE */, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	if(bClear){
		pPkt->byCmd2 = 0;
		pPkt->byData1 = 0;
		pPkt->byData2 = 0;
	}
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	switch(iDirection)
	{
	case PTZ_MOVE_Stop:
		pPkt->byCmd2 = 0;
		pPkt->byData1 = 0;
		pPkt->byData2 = 0;
		break;
	case PTZ_MOVE_Up:
		pPkt->tCmd2.Up = 1;
		pPkt->tCmd2.Down = 0;
		pPkt->byData2 = bySpeed;
		break;
	case PTZ_MOVE_Down:
		pPkt->tCmd2.Up = 0;
		pPkt->tCmd2.Down = 1;
		pPkt->byData2 = bySpeed;
		break;
	case PTZ_MOVE_Left:
		pPkt->tCmd2.Left = 1;
		pPkt->tCmd2.Right = 0;
		pPkt->byData1 = bySpeed;
		break;
	case PTZ_MOVE_Right:
		pPkt->tCmd2.Left = 0;
		pPkt->tCmd2.Right = 1;
		pPkt->byData1 = bySpeed;
		break;
	}
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeFocusFar(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd1.FocusNear = 0;
	pPkt->tCmd1.FocusFar = 1;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeFocusNear(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd1.FocusNear = 1;
	pPkt->tCmd1.FocusFar = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeFocusStop(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeIrisOpen(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.IrisOpen = 1;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeIrisClose(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.IrisClose = 1;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeZoomWide(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd2.ZoomWide = 1;
	pPkt->tCmd2.ZoomTele = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeZoomTele(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd2.ZoomWide = 0;
	pPkt->tCmd2.ZoomTele = 1;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeZoomStop(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->tCmd2.ZoomWide = 0;
	pPkt->tCmd2.ZoomTele = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeAutoScan(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.AutoScan = 1;
	pPkt->byCmd2 = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakePresetCtrl(LPPELCO_P_REQPKT pPkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = nCtrlType;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeSetZoomSpeed(LPPELCO_P_REQPKT pPkt, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = 0x25;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeSetFocusSpeed(LPPELCO_P_REQPKT pPkt, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = 0x27;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeRemoteReset(LPPELCO_P_REQPKT pPkt, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = 0x0F;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeExtCommand(LPPELCO_P_REQPKT pPkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt /* = 0 */, UINT8 byAddress /* = 0 */)
{
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = nCmdType;
	pPkt->byData1 = byDataExt;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}

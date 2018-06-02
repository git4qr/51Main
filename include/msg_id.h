#ifndef  _MSG_ID_H_
#define  _MSG_ID_H_
#pragma once

typedef enum ButtonEvent{
	MSGID_INPUT_TrkCtrl = 0, 		//1
	MSGID_INPUT_Mtd,					//2
	MSGID_INPUT_ZoomLong,		//3
	MSGID_INPUT_ZoomShort,		//4
	MSGID_INPUT_TrkBoxCtrl,		//5
	MSGID_INPUT_TrkSearch,		//6
	MSGID_INPUT_IrisUp,				//7
	MSGID_INPUT_IrisDown,			//8
	MSGID_INPUT_FocusFar,			//9
	MSGID_INPUT_FocusNear, 	//10
	MSGID_INPUT_SensorCtrl,		//11
	//MSGID_INPUT_IMG,
	MSGID_INPUT_Mmt,				//12
	// 3 + 4
	MSGID_INPUT_Max
}Jos_Button;

typedef enum SpecialEvent{
	MSGID_INPUT_AXISX = 0,
	MSGID_INPUT_AXISY = 1,
	MSGID_INPUT_ImgEnh = 3,				//throttle
	MSGID_INPUT__POVX = 5,
	MSGID_INPUT__POVY = 6,

}Jos_Special;

typedef enum mesg_id{
//jos  cmd message
Cmd_Mesg_TrkCtrl=1, 			//1
Cmd_Mesg_Mtd,					//2
Cmd_Mesg_ZoomLong,		//3
Cmd_Mesg_ZoomShort,		//4
Cmd_Mesg_TrkBoxCtrl,		//5
Cmd_Mesg_TrkSearch,			//6
Cmd_Mesg_IrisUp,				//7
Cmd_Mesg_IrisDown,			//8
Cmd_Mesg_FocusFar,			//9
Cmd_Mesg_FocusNear, 		//10
Cmd_Mesg_SensorCtrl, 		//11
Cmd_Mesg_Mmt,					//12
Cmd_Mesg_AIMPOS_X,					//pov
Cmd_Mesg_AIMPOS_Y,					//pov
Cmd_Mesg_AXISX,
Cmd_Mesg_AXISY,
Cmd_Mesg_ImgEnh,
Cmd_Mesg_PresetCtrl,        //18
//ipc
Cmd_IPC_TrkCtrl,

//network
Cmd_Mesg_PALImgEnh,					//20
Cmd_Mesg_SelfTest,
Cmd_Mesg_AxisMove,
Cmd_Mesg_MainElectronicZoom,
Cmd_Mesg_PipElectronicZoom,
Cmd_Mesg_WorkMode,
Cmd_Mesg_Osd,
Cmd_Mesg_SensorMode,
Cmd_Mesg_TVFov,
Cmd_Mesg_PALFov,
Cmd_Mesg_Pan,							//30
Cmd_Mesg_Tilt,
Cmd_Mesg_VideoCompression,
Cmd_Mesg_TrkMode,
Cmd_Mesg_CheckMode,
Cmd_Mesg_EnhMode
//com 
//local
}Cmd_Mesg_ID_Gather;

#endif

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
Cmd_Mesg_TrkCtrl = 1, 		//1
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
Cmd_IPC_Config,
Cmd_IPC_OSD,
Cmd_IPC_UTC,
Cmd_IPC_Camera,

//network
Cmd_Mesg_SelfTest,
Cmd_Mesg_mainVideoSwitch,
Cmd_Mesg_Channel_binding,
Cmd_Mesg_AxisMove,
Cmd_Mesg_MainElectronicZoom,
Cmd_Mesg_PipElectronicZoom,
Cmd_Mesg_saveAxis,
Cmd_Mesg_Picp,
Cmd_Mesg_switchVideoChannel,
Cmd_Mesg_frameCtrl,
Cmd_Mesg_compression_quality,
Cmd_Mesg_wordColor,
Cmd_Mesg_wordType,
Cmd_Mesg_wordSize,
Cmd_Mesg_wordDisEnable,
Cmd_Mesg_config_Write,
Cmd_Mesg_config_Write_Save,
Cmd_Mesg_config_Read,
Cmd_Mesg_jos_kboard,
Cmd_Mesg_WorkMode,
Cmd_Mesg_Osd,
Cmd_Mesg_SensorMode,
Cmd_Mesg_TVFov,
Cmd_Mesg_PALFov,
Cmd_Mesg_Pan,
Cmd_Mesg_Tilt,
Cmd_Mesg_VideoCompression,
Cmd_Mesg_TrkMode,
Cmd_Mesg_CheckMode,
Cmd_Mesg_EnhMode,

//com 
//local
}Cmd_Mesg_ID_Gather;

typedef enum {
	selfTest = 0,
	mainVideo,
	Channel_binding,
	moveAxisX,
	moveAxisY,
	ElectronicZoom,
	saveAxis,
	picp,
	picpChannel,
	switchVideoChannel,
	frameCtrl,
	frameChannel,
	compression_quality,
	compressionChannel,
	wordColor,
	wordType,
	wordSize,
	wordDisEnable,
	config_Wblock,
	config_Wfield,
	config_Wvalue,
	config_Rblock,
	config_Rfield,
	config_Rvalue,
	config_read,
	jos_Kboard,
	jos_Kboard2
}Host_CtrlInput;

typedef enum{
	ACK_selfTest = 0,
	ACK_wordColor,
	ACK_wordType,
	ACK_wordSize,
	NAK_wordColor,
	NAK_wordType,
	NAK_wordSize,
	ACK_mainVideoStatus,
	ACK_Channel_bindingStatus,  //
	ACK_avtTrkStatus,
	ACK_avtTrkType,
	ACK_avtErrorOutput,
	ACK_mmtStatus,
	ACK_mmtSelectStatus,  //
	ACK_EnhStatus,
	ACK_posMoveStatus,   //
	ACK_moveAxisStatus,  //
	ACK_ElectronicZoomStatus,
	ACK_picpStatus,
	ACK_VideoChannelStatus,
	ACK_frameCtrlStatus,
	ACK_compression_quality,
	ACK_config_Write,
	ACK_config_Reab,
	ACK_jos_Kboard,
}ACK_Host_CtrlInput;

typedef enum{
	ACK_selfTest2 = 0,  //
	ACK_Channel_bindingStatus2,  //
	ACK_posMoveStatus2,    //
	ACK_moveAxisStatus2,   //
	ACK_mmtSelectStatus2,   //
	ACK_config_Wblock,
	ACK_config_Wfield,
	ACK_config_Wvalue,
	ACK_config_Rblock,
	ACK_config_Rfield,
	ACK_config_Rvalue,
	ACK_wordColor2,   //
	ACK_wordType2,  //
	ACK_wordSize2,		//
	NAK_wordColor2,	//
	NAK_wordType2,	//
	NAK_wordSize2,		//
	ACK_jos_Kboard2,		//
}ACK_Host_Value;
#endif

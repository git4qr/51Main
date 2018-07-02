
#include "joystickProcess.h"
int joystick_fd = -1;
int IrisAndFocus_Ret = 0;
int MmtSelect_Ret = 0;
bool Mmt_sign = true;
#define MmtMax 6;

bool CJosStick::JosStart = true;

CJosStick::CJosStick()
{
	jse = NULL;
	isTrackIn=false;
}

CJosStick::~CJosStick()
{
	Destroy();
}

void CJosStick::updateJosKeyMap()
{
	int * p = &m_josBtn_t.jos_btn_1;
	for(int i = 0 ;i<MSGID_INPUT_Max;i++)
		josKeyMap[i] = *(p+i);
#if 0
	printf("*****************************\n");
	for(int i=0;i<MSGID_INPUT_Max;i++)
		printf("josKeyMap[%d] = %d\n",i,josKeyMap[i]);
	printf("*****************end*********\n");
#endif
}

void CJosStick::updateJosAxisMap()
{
	int *p = &m_josAxis_t.jos_axis_1_x;
	for(int i = 0;i<6;i++)
		JosAxisMap[i] = *(p+i);
}

int  CJosStick::Create()
{
	//preinitial();
	 open_joystick(joystick_Dev);
	 jse=(joy_event*)malloc (sizeof(joy_event));
	 updateJosKeyMap();
	 updateJosAxisMap();
	 return 0;
}

int CJosStick::open_joystick(char *joystick_device)
{
   // joystick_fd = open(joystick_Dev, O_RDONLY | O_NONBLOCK);
	 joystick_fd = open(joystick_Dev, O_RDONLY);

    if (joystick_fd < 0)
    {
    	fprintf(stdout,"jos_dev open failed\n");
        return joystick_fd;
    }
		return joystick_fd;
}

int CJosStick::read_joystick_event(joy_event *jse)
{
   int  bytes = read(joystick_fd, jse, sizeof(*jse));

    if (bytes == -1)
        return 0;

    if (bytes == sizeof(*jse))
        return 1;

}
# if 0
//1080P
int CJosStick::JosToWinX(int x)
{
	int m_WinX;
	m_WinX = x/34 + ShowDPI[0][0]/2;
	return m_WinX;
}

int CJosStick::JosToWinY(int y)
{
	int m_WinY;
	m_WinY = y/60 + ShowDPI[0][1]/2;
	return m_WinY;
}
#else
//720p
int CJosStick::JosToWinX(int x)
{
	int m_WinX;
	m_WinX = x/91 + ShowDPI[1][0]/2;
	return m_WinX;
}

int CJosStick::JosToWinY(int y)
{
	int m_WinY;
	m_WinY = y/113 + ShowDPI[1][1]/2;
	return m_WinY;
}
#endif
int  CJosStick::Stop()
{
    close(joystick_fd);
    return 0;
}

int  CJosStick::Destroy()
{
	JosStart = false;
	OSA_thrDelete(&m_thrJoy);
	Stop();
	return 0;
}

int  CJosStick::Config()
{
return 0;
}

int  CJosStick::Run()
{
	int iRet;
	iRet = OSA_thrCreate(&m_thrJoy, josEventFunc , 0, 0, (void*)this);
		if(iRet != 0)
	fprintf(stdout,"josthread creat failed\n");
	return 0;
}

void CJosStick::procJosEvent_Axis(UINT8  mjosNum )
{

#if 1
	if(mjosNum > 6 || mjosNum == 4)
		return ;
	//printf("fresh  mjosNum = %d \n",mjosNum);
	if(mjosNum == 5 || mjosNum == 6 )
		mjosNum -= 3;
	else if(mjosNum == 3)
		mjosNum = 4;

	int id = JosAxisMap[mjosNum];
#endif

#if 0
	printf("!!!  id = %d , nJos = %d \n",id,mjosNum);
	printf("*****************************\n");
	for(int i=0;i<6;i++)
		printf("josAxisMap[%d] = %d\n",i,JosAxisMap[i]);
	printf("*****************end*********\n");
#endif

	switch(id){

			case MSGID_INPUT_AXISX:
				if(EXT_Ctrl[MSGID_INPUT_IrisAndFocusAndExit] == 0 &&  EXT_Ctrl[Cmd_Mesg_Mmt - 1] == 0){
				EXT_Ctrl[14] = jse->value;
					AXIS_X();
				}
				else if(EXT_Ctrl[Cmd_Mesg_Mmt - 1] == 1){
					X_CtrlMmtSelect(jse->value);
					EnableMmtSelect();
				}
					break;
				case MSGID_INPUT_AXISY:
					if(EXT_Ctrl[MSGID_INPUT_IrisAndFocusAndExit] == 0 && EXT_Ctrl[Cmd_Mesg_Mmt - 1] == 0){
					EXT_Ctrl[15] = jse->value;
					AXIS_Y();
					}
					else {
						Y_CtrlIrisAndFocus(jse->value);
						ENableIrisAndFocusAndExit();
					}
					break;
				case MSGID_INPUT__POVX:
					if(jse->value == 0){
						EXT_Ctrl[12] = 0;
						AIMPOS_X();
						}
					else if(jse->value == -32767){
						EXT_Ctrl[12] = 1;
						AIMPOS_X();
					}
					else if(jse->value == 32767){
						EXT_Ctrl[12] = 2;
						AIMPOS_X();
					}
					break;
				case MSGID_INPUT__POVY:
					if(jse->value == 0){
						EXT_Ctrl[13] = 0;
						AIMPOS_Y();
					}
					else if(jse->value == -32767){
						EXT_Ctrl[13] = 1;
						AIMPOS_Y();
					}
					else if(jse->value == 32767){
						EXT_Ctrl[13] = 2;
						AIMPOS_Y();
					}
					break;
				case MSGID_INPUT_ImgEnh:
					if(jse->value < -32760){
						if(EXT_Ctrl[16] == 1)
						EnableIMG();
					}
					else if(jse->value > 32760){
						if(EXT_Ctrl[16] ==0)
						EnableIMG();
					}
					break;
				default:
					break;
	 }

}

void CJosStick::X_CtrlMmtSelect(int value)
{
	if(Mmt_sign){
		if(value > 10000){
			MmtSelect_Ret = (MmtSelect_Ret + 1) % MmtMax;
		if(MmtSelect_Ret == 0)
			MmtSelect_Ret = 1;
		Mmt_sign = false;
		}
	}

	if(value < 2000 && value > -2000)
		Mmt_sign = true;

	if(Mmt_sign){
		if(value < -10000){
			MmtSelect_Ret = (MmtSelect_Ret - 1) % MmtMax;
			if(MmtSelect_Ret == 0)
				MmtSelect_Ret = 5;
			Mmt_sign = false;
		}

	}
	EXT_Ctrl[14] = MmtSelect_Ret;
}

void CJosStick::Y_CtrlIrisAndFocus(int value)
{
	if(value < -10000)
		EXT_Ctrl[15] = -1;
	else if(value > 20000)
		EXT_Ctrl[15] = 1;
	else
		EXT_Ctrl[15] = 0;
}

void CJosStick::ProcJosEvent_Button(UINT8  njosNum)
{
#if 1
	if(njosNum >= MSGID_INPUT_Max)
		return ;
	int id = josKeyMap[njosNum];
#endif

#if 0
	printf("!!!  id = %d , nJos = %d \n",id,njosNum);
	printf("*****************************\n");
	for(int i=0;i<MSGID_INPUT_Max;i++)
		printf("josKeyMap[%d] = %d\n",i,josKeyMap[i]);
	printf("*****************end*********\n");
#endif

	switch (id) {
    		case MSGID_INPUT_TrkCtrl:
    				if(jse->value == 1){
    							EnableTrk();
    				}
    				break;
    		case MSGID_INPUT_Mtd:
    				if(jse->value == 1){
    					EnableMtd();
    				}
    				break;
    		case MSGID_INPUT_ZoomLong:
    				if(jse->value == 1)
    					EXT_Ctrl[MSGID_INPUT_ZoomLong ] = 1;
    				else
    					EXT_Ctrl[MSGID_INPUT_ZoomLong ] = 0;
    				ZoomLongCtrl();
    				break;
    		case MSGID_INPUT_ZoomShort:
    			if(jse->value == 1)
    				EXT_Ctrl[MSGID_INPUT_ZoomShort ] = 1;
    			else
    				EXT_Ctrl[MSGID_INPUT_ZoomShort ] = 0;
    			ZoomShortCtrl();
    			break;
    		case MSGID_INPUT_IrisAndFocusAndExit:
    			if(jse->value == 1){
    				IrisAndFocus_Ret =  (IrisAndFocus_Ret + 1) % 3;
    				EXT_Ctrl[MSGID_INPUT_IrisAndFocusAndExit] =IrisAndFocus_Ret;
    				ENableIrisAndFocusAndExit();
    			}
    			break;
    		case MSGID_INPUT_TrkSearch:
    			if(jse->value == 1){
    					EnableTrkSearch();
    					EXT_Ctrl[MSGID_INPUT_TrkSearch ] = 1;
    			}
    			else{
    				EnableTrkSearch();
    				EXT_Ctrl[MSGID_INPUT_TrkSearch ] = 0;
    			}
    			break;
    		case MSGID_INPUT_FuncMenu:
    			if(jse->value == 1){
    				EXT_Ctrl[MSGID_INPUT_FuncMenu] = 1;
    				EnableFuncMenu();
    			}
    			else{
    				EXT_Ctrl[MSGID_INPUT_FuncMenu] = 0;
    				EnableFuncMenu();
    			}
    			break;
    		case MSGID_INPUT_IrisDown:
    				if(jse->value == 1){
    					EXT_Ctrl[MSGID_INPUT_IrisDown] = 1;
    					IrisDown();
    				}
    				else{
    					EXT_Ctrl[MSGID_INPUT_IrisDown] = 0;
    					IrisDown();
    				}
    				break;
    		case MSGID_INPUT_FocusFar:
    			if(jse->value == 1){
    				EXT_Ctrl[MSGID_INPUT_FocusFar ] = 1;
    					FocusUp();
    			}
    			else{
    				EXT_Ctrl[MSGID_INPUT_FocusFar] = 0;
    				FocusUp();
    			}
    			break;
    		case MSGID_INPUT_FocusNear:
    			if(jse->value == 1){
    				EXT_Ctrl[MSGID_INPUT_FocusNear] = 1;
    					FocusDown();
    			}
    			else{
    				EXT_Ctrl[MSGID_INPUT_FocusNear] = 0;
    					FocusDown();
    			}
    			break;
    		case MSGID_INPUT_SensorCtrl:
    				if(jse->value == 1){
						SwitchSensor();
    				}
    				break;
    			case MSGID_INPUT_Mmt:
    				if(jse->value == 1){
    						EnableMmt();
    				}
    				break;
    			default:
    				break;
    }
}

void CJosStick::JoystickProcess()
{
    int rc;
    if (rc = read_joystick_event(jse) == 1) {
        jse->type &= ~JS_EVENT_INIT; /* ignore synthetic events */
        switch(jse->type){
        		case   JS_EVENT_AXIS:
        			      procJosEvent_Axis(jse->number);
                          break;
        		case   JS_EVENT_BUTTON:
        			    ProcJosEvent_Button(jse->number);
        				break;
        		default:
        		     printf("INFO: ERROR Jos Event, Can not excute here!!!\r\n");

           }
    }
}



#if 0



void CJosStick::ProcEvent_Button(int  EventBtnNum, int EventValue)
{
    switch (EventBtnNum) {
    		case MSGID_INPUT_TrkCtrl:
    				if(EventValue == 1){
    							EnableTrk();
    				}
    				break;
    		case MSGID_INPUT_Mtd:
    				if(EventValue == 1){
    			/*		if(JOS_Value[17] == 0)
    					JOS_Value[17] = 1;
    				else
    					JOS_Value[17] = 0;
    					PresetCtrl();
    					*/
    					EnableMtd();
    				}
    				break;
    		case MSGID_INPUT_ZoomLong:
    				if(EventValue == 1){
    					EXT_Ctrl[MSGID_INPUT_ZoomLong ] = 1;
    				}
    				else{
    					EXT_Ctrl[MSGID_INPUT_ZoomLong ] = 0;
    				ZoomLongCtrl();
    				}
    				break;
    		case MSGID_INPUT_ZoomShort:
    			if(EventValue == 1)
    				EXT_Ctrl[MSGID_INPUT_ZoomShort ] = 1;
    			else
    				EXT_Ctrl[MSGID_INPUT_ZoomShort ] = 0;
    			ZoomShortCtrl();
    			break;
    		case MSGID_INPUT_TrkBoxCtrl:
    			if(EventValue == 1){
    					TrkBoxCtrl();
    			}
    			break;
    		case MSGID_INPUT_TrkSearch:
    			if(EventValue== 1){
    					EnableTrkSearch();
    					EXT_Ctrl[MSGID_INPUT_TrkSearch ] = 1;
    			}
    			else{
    				EnableTrkSearch();
    				EXT_Ctrl[MSGID_INPUT_TrkSearch ] = 0;
    			}
    			break;
    		case MSGID_INPUT_IrisUp:
    			if(EventValue == 1){
    				EXT_Ctrl[MSGID_INPUT_IrisUp ] = 1;
    					IrisUp();
    			}
    			else{
    				EXT_Ctrl[MSGID_INPUT_IrisUp] = 0;
    					IrisUp();
    			}
    			break;
    		case MSGID_INPUT_IrisDown:
    				if(EventValue == 1){
    					EXT_Ctrl[MSGID_INPUT_IrisDown] = 1;
    					IrisDown();
    				}
    				else{
    					EXT_Ctrl[MSGID_INPUT_IrisDown] = 0;
    					IrisDown();
    				}
    				break;
    		case MSGID_INPUT_FocusFar:
    			if(EventValue== 1){
    				EXT_Ctrl[MSGID_INPUT_FocusFar ] = 1;
    					FocusUp();
    			}
    			else{
    				EXT_Ctrl[MSGID_INPUT_FocusFar] = 0;
    				FocusUp();
    			}
    			break;
    		case MSGID_INPUT_FocusNear:
    			if(EventValue == 1){
    				EXT_Ctrl[MSGID_INPUT_FocusNear] = 1;
    					FocusDown();
    			}
    			else{
    				EXT_Ctrl[MSGID_INPUT_FocusNear] = 0;
    					FocusDown();
    			}
    			break;
    		case MSGID_INPUT_SensorCtrl:
    				if(EventValue == 1){
						SwitchSerson();
    				}
    				break;
    			case MSGID_INPUT_Mmt:
    				if(EventValue == 1){
    						EnableMmt();
    				}
    				break;
    			default:
    				break;
    }
#if 0
    switch (EventBtnNum) {

    		case MSGID_INPUT_TrkCtrl:
    				if(EventValue == 1){
    					if(ExtInputCtrl[0] == 0){
    						ExtInputCtrl[0] = 1;
    						EnableTrk();
    					}
    					else{
    						ExtInputCtrl[0] = 0;
    							EnableTrk();
    						}
    				}
    				break;
    		case MSGID_INPUT_SensorCtrl:
    				if(EventValue == 1){
    					SensorStat = (SensorStat + 1)%eSen_Max;
    					ExtInputCtrl[1] = SensorStat;
    					SelSensor();
    				}
    				break;
    		case MSGID_INPUT_ZoomLong:
    				if(EventValue == 1){
    					ExtInputCtrl[2] = 1;
    				}
    				else
    					ExtInputCtrl[2] = 0;
    				ZoomLongCtrl();
    				break;
    		case MSGID_INPUT_ZoomShort:
    			if(EventValue == 1){
    				ExtInputCtrl[3] = 1;
    			}
    			else
    				ExtInputCtrl[3] = 0;
    			ZoomShortCtrl();
    			break;
    		case MSGID_INPUT_TrkBoxCtrl:
    			if(EventValue == 1){
    					AvtTrkAimSize = (AvtTrkAimSize +1)%Trk_SizeMax;
    					ExtInputCtrl[4] = AvtTrkAimSize;
    					TrkBoxCtrl();
    			}
    			break;
    		case MSGID_INPUT_TrkSearch:
    			if(EventValue == 1){
    				ExtInputCtrl[5] = 1;
    					EnableTrkSearch();
    			}
    			else{
    				ExtInputCtrl[5] = 0;
    					EnableTrkSearch();
    			}
    			break;
    		case MSGID_INPUT_IrisUp:
    			if(EventValue== 1){
    				ExtInputCtrl[6] = 1;
    					IrisUp();
    			}
    			else{
    				ExtInputCtrl[6] = 0;
    					IrisUp();
    			}
    			break;
    		case MSGID_INPUT_IrisDown:
    				if(EventValue== 1){
    					ExtInputCtrl[7] = 1;
    					IrisDown();
    				}
    				else{
    					ExtInputCtrl[7] = 0;
    					IrisDown();
    				}
    				break;
    		case MSGID_INPUT_FocusFar:
    			if(EventValue == 1){
    				ExtInputCtrl[8] = 1;
    					FocusUp();
    			}
    			else{
    				ExtInputCtrl[8] = 0;
    				FocusUp();
    			}
    			break;
    		case MSGID_INPUT_FocusNear:
    			if(EventValue == 1){
    				ExtInputCtrl[9] = 1;
    					FocusDown();
    			}
    			else{
    				ExtInputCtrl[9] = 0;
    					FocusDown();
    			}
    			break;
    		case MSGID_INPUT_IMG:
    				if(EventValue == 1){
    						if(ExtInputCtrl[10] == 0){
    							ExtInputCtrl[10] = 1;
    							EnableIMG();
    				}
    				else{
    					ExtInputCtrl[10] = 0;
    						EnableIMG();
    					}
    				}
    				break;
    			case MSGID_INPUT_unable:
    				break;
    			default:
    				break;
    }
#endif
}


void CJosStick::Button_Event(u_int8_t  btnNum)
{
	int   idFlg;
	ExtEvent   btnEvent;
    btnEvent.eventValue =jse->value;
    printf("INFO: get btn info num%d---value%d\r\n",btnNum+1,btnEvent.eventValue);
    idFlg=finderMapKey(btnNum+1);
    printf("INFO: idFlag ---%d\r\n",idFlg);
    ProcEvent_Button(idFlg,  btnEvent.eventValue );
}

void CJosStick::procJosEvent_Axis(UINT8  mjosNum )
{
	switch(mjosNum){

			case MSGID_INPUT_AXISX:
				ExtInputCtrl[14] = jse->value;
					AXIS_X();
					break;
				case MSGID_INPUT_AXISY:
					ExtInputCtrl[15] = jse->value;
					AXIS_Y();
					break;
				case MSGID_INPUT__POVX:
					if(!(jse->value == 0)){
						ExtInputCtrl[12] = jse->value;
						AIMPOS_X();
						}
					break;
				case MSGID_INPUT__POVY:
					if(!(jse->value == 0)){
						ExtInputCtrl[13] = jse->value;
						AIMPOS_Y();
					}
					break;
				default:
					break;
	 }
}

void CJosStick::JoystickProcess()
{
    int rc;
    if (rc = read_joystick_event(jse) == 1) {
        jse->type &= ~JS_EVENT_INIT; /* ignore synthetic events */
        switch(jse->type){
        		case   JS_EVENT_AXIS:
        			     procJosEvent_Axis(jse->number);
        			    //  processEventBtn(jse->number);
                          break;
        		case   JS_EVENT_BUTTON:
        			 //  ProcJosEvent_Button(jse->number);
        			  Button_Event(jse->number);
        				break;
        		default:
        		     printf("INFO: ERROR Jos Event, Can not excute here!!!\r\n");

           }
    }
}
#endif


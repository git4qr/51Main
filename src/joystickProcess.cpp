
#include "joystickProcess.h"
static	int joystick_fd = -1;

bool CJosStick::JosStart = 1;

CJosStick::CJosStick()
{
	jse = NULL;
	//JosStart = true;s
}

CJosStick::~CJosStick()
{
	Destroy();
}
void CJosStick::Init()
{

}

void CJosStick::Create()
{
	 open_joystick(joystick_Dev);
	 jse=(joy_event*)malloc (sizeof(joy_event));
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
    int bytes;

    bytes = read(joystick_fd, jse, sizeof(*jse));

    if (bytes == -1)
        return 0;

    if (bytes == sizeof(*jse))
        return 1;

}

void CJosStick::Stop()
{
    close(joystick_fd);
}

void CJosStick::Destroy()
{
	JosStart = false;
	OSA_thrDelete(&m_thrJoy);
	Stop();
}

void CJosStick::Config()
{

}

void CJosStick::Run()
{
	int iRet;
	iRet = OSA_thrCreate(&m_thrJoy, josEventFunc , 0, 0, (void*)this);
		if(iRet != 0)
	fprintf(stdout,"josthread creat failed\n");
}


void CJosStick::JoystickProcess()
{
    int rc;
    if (rc = read_joystick_event(jse) == 1) {
        jse->type &= ~JS_EVENT_INIT; /* ignore synthetic events */
        if (jse->type == JS_EVENT_AXIS) {
            switch (jse->number) {
            case MSGID_INPUT_AXISX:
           		JOS_Value[14] = jse->value;
           		AXIS_X();
                break;
            case MSGID_INPUT_AXISY:
           		JOS_Value[15] = jse->value;
           		AXIS_Y();
                break;
            case MSGID_INPUT__POVX:
            	if(!(jse->value == 0))
            	{
           		JOS_Value[12] = jse->value;
           		AIMPOS_X();
            	}
                break;
            case MSGID_INPUT__POVY:
            	if(!(jse->value == 0))
            	{
           		JOS_Value[13] = jse->value;
           		AIMPOS_Y();
            	}
                break;
            default:
                break;
            }
        } 
			else if (jse->type == JS_EVENT_BUTTON) {
                switch (jse->number) {
                	case MSGID_INPUT_TrkCtrl:
                		if(jse->value == 1){
					 	if(JOS_Value[0] == 0){
					 		JOS_Value[0] = 1;
					 	EnableTrk(true);
					 	}
					 	else{
					 		JOS_Value[0] = 0;
					 	EnableTrk(false);
					 	}
                		}
						break;
                	case MSGID_INPUT_SensorCtrl:
                		if(jse->value == 1){
                			SensorStat = (SensorStat + 1)%eSen_Max;
                			JOS_Value[1] = SensorStat;
					 		SelSensor();
					 	}
                		break;
                	case MSGID_INPUT_ZoomLong:
                		if(jse->value == 1){
					 		JOS_Value[2] = 1;
                		}
					 	else
					 		JOS_Value[2] = 0;
					 	ZoomLongCtrl();
                		break;
                	case MSGID_INPUT_ZoomShort:
                		if(jse->value == 1){
					 		JOS_Value[3] = 1;
                		}
					 	else
					 		JOS_Value[3] = 0;
					 	ZoomShortCtrl();
                    	break;
                	case MSGID_INPUT_TrkBoxCtrl:
                		if(jse->value == 1){
                			AvtTrkAimSize = (AvtTrkAimSize +1)%Trk_SizeMax;
                			JOS_Value[4] = AvtTrkAimSize;
                			TrkBoxCtrl();
                		}
                    	break;
                	case MSGID_INPUT_TrkSearch:
                		if(jse->value == 1){
					 		JOS_Value[5] = 1;
					 		EnableTrkSearch(true);
					 	}
					 	else{
					 		JOS_Value[5] = 0;
					 		EnableTrkSearch(false);
					 	}
                    	break;
                	case MSGID_INPUT_IrisUp:
                		if(jse->value == 1){
					 		JOS_Value[6] = 1;
					 		IrisUp(true);
					 	}
					 	else{
					 		JOS_Value[6] = 0;
					 		IrisUp(false);
					 	}
                    	break;
                	case MSGID_INPUT_IrisDown:
                		if(jse->value == 1){
					 		JOS_Value[7] = 1;
					 		IrisDown(true);
					 	}
					 	else{
					 		JOS_Value[7] = 0;
					 		IrisDown(false);
					 	}
                    	break;
                	case MSGID_INPUT_FocusFar:
                		if(jse->value == 1){
					 		JOS_Value[8] = 1;
					 		FocusUp(true);
					 	}
					 	else{
					 		JOS_Value[8] = 0;
					 		FocusUp(false);
					 	}
                    	break;
                	case MSGID_INPUT_FocusNear:
                		if(jse->value == 1){
					 		JOS_Value[9] = 1;
					 		FocusDown(true);
					 	}
					 	else{
					 		JOS_Value[9] = 0;
					 		FocusDown(false);
                		}
                    	break;
                	case MSGID_INPUT_IMG:
                		if(jse->value == 1){
					 	if(JOS_Value[10] == 0){
					 		JOS_Value[10] = 1;
					 		EnableIMG(true);
					 	}
					 	else{
					 		JOS_Value[10] = 0;
					 		EnableIMG(false);
					 	}
                		}
                    	break;
                	case MSGID_INPUT_unable:

                    	break;
                	default:
                		break;
                }
        }

    }

}


	

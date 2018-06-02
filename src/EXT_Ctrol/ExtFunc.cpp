#include "ExtFunc.h"
#include "UserProcess.h"

extern CUserBase  *Sthis;

void track(void)
{
	Sthis->EnableTrk();
}

void SelSensor(void)
{
	//Sthis->SelSensor();
}

void TrkBoxCtrl(void)
{
	Sthis->TrkBoxCtrl();
}

void TrkSearch(void)
{
	Sthis->	EnableTrkSearch();
}

void upIris(void)
{
	Sthis->IrisUp();
}

void downIris(void)
{
	Sthis->IrisDown();
}

void focusUp(void)
{
	Sthis->FocusUp();
}

void FocusDown(void)
{
	Sthis->FocusDown();
}

void imgEnhance(void)
{
	Sthis->EnableIMG();
}

void autoIsriFocus(void)
{
	//Sthis->EnableAuto_Iris_Focus();
}

void aimposx(void)
{
	Sthis->AIMPOS_X();
}

void aimposy(void)
{
	Sthis->AIMPOS_Y();
}

void zoomlong(void)
{
	Sthis->ZoomLongCtrl();
}
void zoomshort(void)
{
	Sthis->ZoomShortCtrl();
}

void axisXctrl(void)
{
	Sthis->AXIS_X();
}

void axisYctrl(void)
{
	Sthis->AXIS_Y();
}






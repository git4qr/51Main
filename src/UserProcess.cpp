#include "UserProcess.h"

CUserBase::CUserBase()
{
 	JOS_Value = new int[20];
 	memset(JOS_Value, 0, sizeof(int) * 20);
}

CUserBase::~CUserBase()
{
	Destroy();
}



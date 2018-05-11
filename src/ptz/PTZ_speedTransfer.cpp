#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "osa.h"
#include "PTZ_speedTransfer.h"

#define FILL_TAB(TAB, INDEX, MIN, MAX, COUNT)	\
{		\
	minV = MIN;	maxV = MAX;		\
	dt = (maxV-minV)/COUNT;		\
	for(i=0; i<COUNT; i++){		\
		TAB##SpeedTab[INDEX+i] = minV + (i*dt);	\
	}		\
}

CPTZSpeedTransfer::CPTZSpeedTransfer()
{
	m_bTabInit = FALSE;
	memset(m_PanSpeedTab,0,sizeof(m_PanSpeedTab));
	memset(m_TilSpeedTab,0,sizeof(m_TilSpeedTab));
}
CPTZSpeedTransfer::~CPTZSpeedTransfer()
{
	m_bTabInit = FALSE;
}
int CPTZSpeedTransfer::SpeedTabInit()
{

	int i;
	int dt, minV, maxV;

	if(m_bTabInit)
		return 0;

#if 0//���1
	m_PanSpeedTab[0] = 0;
	m_PanSpeedTab[1] = 490;
	m_PanSpeedTab[2] = 490;
	m_PanSpeedTab[3] = 490;
	m_PanSpeedTab[4] = 490;
	m_PanSpeedTab[5] = 490;
	m_PanSpeedTab[6] = 490;
	m_PanSpeedTab[7] = 490;
	m_PanSpeedTab[8] = 490;
	m_PanSpeedTab[9] = 490;
	m_PanSpeedTab[10] = 590;
	m_PanSpeedTab[11] = 690;
	m_PanSpeedTab[12] = 700;
	m_PanSpeedTab[13] = 780;
	m_PanSpeedTab[14] = 890;
	m_PanSpeedTab[15] = 990;
	m_PanSpeedTab[16] = 1000;
	m_PanSpeedTab[17] = 1090;
	m_PanSpeedTab[18] = 1290;
	m_PanSpeedTab[19] = 1390;
	m_PanSpeedTab[20] = 1480;
	m_PanSpeedTab[21] = 1690;
	m_PanSpeedTab[22] = 1790;
	m_PanSpeedTab[23] = 1990;
	m_PanSpeedTab[24] = 2190;
	m_PanSpeedTab[25] = 2390;
	m_PanSpeedTab[26] = 2600;
	m_PanSpeedTab[27] = 2890;
	m_PanSpeedTab[28] = 3190;
	m_PanSpeedTab[29] = 3490;

	m_PanSpeedTab[30] = 3690;
	m_PanSpeedTab[31] = 3690;
	m_PanSpeedTab[32] = 3690;
	m_PanSpeedTab[33] = 5790;
	m_PanSpeedTab[34] = 5790;
	m_PanSpeedTab[35] = 5980;
	m_PanSpeedTab[36] = 6490;
	m_PanSpeedTab[37] = 6490;
	m_PanSpeedTab[38] = 9290;
	m_PanSpeedTab[39] = 9290;

	m_PanSpeedTab[40] = 9590;
	m_PanSpeedTab[41] = 10480;
	m_PanSpeedTab[42] = 11490;
	m_PanSpeedTab[43] = 12590;
	m_PanSpeedTab[44] = 13880;
	m_PanSpeedTab[45] = 15190;
	m_PanSpeedTab[46] = 16690;
	m_PanSpeedTab[47] = 18290;
	m_PanSpeedTab[48] = 19980;
	m_PanSpeedTab[49] = 21980;

	m_PanSpeedTab[50] = 24080;
	m_PanSpeedTab[51] = 26400;
	m_PanSpeedTab[52] = 29000;
	m_PanSpeedTab[53] = 31790;
	m_PanSpeedTab[54] = 34900;
	m_PanSpeedTab[55] = 38170;
	m_PanSpeedTab[56] = 41830;
	m_PanSpeedTab[57] = 45920;
	m_PanSpeedTab[58] = 50400;
	m_PanSpeedTab[59] = 55290;

	m_PanSpeedTab[60] = 60760;
	m_PanSpeedTab[61] = 66540;
	m_PanSpeedTab[62] = 73040;
	m_PanSpeedTab[63] = 80040;

	TiltSpeedTab[0] = 0;
	for(i=1; i<=6; i++){
		TiltSpeedTab[i] = 500;
	}
	TiltSpeedTab[7] = 900;
	TiltSpeedTab[8] = 1300;
	TiltSpeedTab[9] = 1600;
	FULL_TAB(Tilt, 10,   2000,  3700, 5);
	FULL_TAB(Tilt, 15,   3700,  5600, 5);
	FULL_TAB(Tilt, 20,   5600,  7000, 5);
	FULL_TAB(Tilt, 25,   7000,  9400, 5);
	FULL_TAB(Tilt, 30,   9400,  11700, 5);
	FULL_TAB(Tilt, 35,   11700,  14100, 5);
	FULL_TAB(Tilt, 40,   14100,  17000, 5);
	FULL_TAB(Tilt, 45,   17000,  20400, 5);
	FULL_TAB(Tilt, 50,   20400,  25400, 5);
	FULL_TAB(Tilt, 55,   25400,  29400, 5);
	TiltSpeedTab[60] = 29400;
	TiltSpeedTab[61] = 37200;
	TiltSpeedTab[62] = 39800;
	TiltSpeedTab[63] = 44500;

#endif//���1

#if 1
	//FULL_TAB(Pan, 0,  0,  2000, 10);
	FILL_TAB(m_Pan, 0,  0,  8000, 64);
	//FULL_TAB(Tilt, 0,  0,  1000, 10);
	FILL_TAB(m_Til, 0,  0, 4000, 64);
#endif

#if 1 //ԭʼ
	//int i;
	//int dt, minV, maxV;

	m_PanSpeedTab[0] = 0;
#if 1
	m_PanSpeedTab[1] = 49;
	m_PanSpeedTab[2] = 49;
	m_PanSpeedTab[3] = 49;
	m_PanSpeedTab[4] = 49;
	m_PanSpeedTab[5] = 49;
	m_PanSpeedTab[6] = 49;
	m_PanSpeedTab[7] = 49;
	m_PanSpeedTab[8] = 49;
	m_PanSpeedTab[9] = 49;
#else
	m_PanSpeedTab[1] = 5;
	m_PanSpeedTab[2] = 10;
	m_PanSpeedTab[3] = 15;
	m_PanSpeedTab[4] = 20;
	m_PanSpeedTab[5] = 25;
	m_PanSpeedTab[6] = 30;
	m_PanSpeedTab[7] = 35;
	m_PanSpeedTab[8] = 40;
	m_PanSpeedTab[9] = 45;
#endif
	m_PanSpeedTab[10] = 59;
	m_PanSpeedTab[11] = 69;
	m_PanSpeedTab[12] = 70;
	m_PanSpeedTab[13] = 78;
	m_PanSpeedTab[14] = 89;
	m_PanSpeedTab[15] = 99;
	m_PanSpeedTab[16] = 100;
	m_PanSpeedTab[17] = 109;
	m_PanSpeedTab[18] = 129;
	m_PanSpeedTab[19] = 139;
	m_PanSpeedTab[20] = 148;
	m_PanSpeedTab[21] = 169;
	m_PanSpeedTab[22] = 179;
	m_PanSpeedTab[23] = 199;
	m_PanSpeedTab[24] = 219;
	m_PanSpeedTab[25] = 239;
	m_PanSpeedTab[26] = 260;
	m_PanSpeedTab[27] = 289;
	m_PanSpeedTab[28] = 319;
	m_PanSpeedTab[29] = 349;

	m_PanSpeedTab[30] = 369;
	m_PanSpeedTab[31] = 369;
	m_PanSpeedTab[32] = 369;
	m_PanSpeedTab[33] = 579;
	m_PanSpeedTab[34] = 579;
	m_PanSpeedTab[35] = 598;
	m_PanSpeedTab[36] = 649;
	m_PanSpeedTab[37] = 649;
	m_PanSpeedTab[38] = 929;
	m_PanSpeedTab[39] = 929;

	m_PanSpeedTab[40] = 959;
	m_PanSpeedTab[41] = 1048;
	m_PanSpeedTab[42] = 1149;
	m_PanSpeedTab[43] = 1259;
	m_PanSpeedTab[44] = 1388;
	m_PanSpeedTab[45] = 1519;
	m_PanSpeedTab[46] = 1669;
	m_PanSpeedTab[47] = 1829;
	m_PanSpeedTab[48] = 1998;
	m_PanSpeedTab[49] = 2198;

	m_PanSpeedTab[50] = 2408;
	m_PanSpeedTab[51] = 2640;
	m_PanSpeedTab[52] = 2900;
	m_PanSpeedTab[53] = 3179;
	m_PanSpeedTab[54] = 3490;
	m_PanSpeedTab[55] = 3817;
	m_PanSpeedTab[56] = 4183;
	m_PanSpeedTab[57] = 4592;
	m_PanSpeedTab[58] = 5040;
	m_PanSpeedTab[59] = 5529;

	m_PanSpeedTab[60] = 6076;
	m_PanSpeedTab[61] = 6654;
	m_PanSpeedTab[62] = 7304;
	m_PanSpeedTab[63] = 8004;

	m_TilSpeedTab[0] = 0;
	for(i=1; i<=6; i++){
		m_TilSpeedTab[i] = 50;
	}
	m_TilSpeedTab[7] = 90;
	m_TilSpeedTab[8] = 130;
	m_TilSpeedTab[9] = 160;
	FILL_TAB(m_Til, 10,   200,  370, 5);
	FILL_TAB(m_Til, 15,   370,  560, 5);
	FILL_TAB(m_Til, 20,   560,  700, 5);
	FILL_TAB(m_Til, 25,   700,  940, 5);
	FILL_TAB(m_Til, 30,   940,  1170, 5);
	FILL_TAB(m_Til, 35,   1170,  1410, 5);
	FILL_TAB(m_Til, 40,   1410,  1700, 5);
	FILL_TAB(m_Til, 45,   1700,  2040, 5);
	FILL_TAB(m_Til, 50,   2040,  2540, 5);
	FILL_TAB(m_Til, 55,   2540,  2940, 5);
	m_TilSpeedTab[60] = 2940;
	m_TilSpeedTab[61] = 3720;
	m_TilSpeedTab[62] = 3980;
	m_TilSpeedTab[63] = 4450;

#endif

	m_bTabInit = TRUE;

	return 0;
}

int CPTZSpeedTransfer::GetPanSpeed(int iIn)
{
	int iOut;
	if(iIn == 0)
		return 0;

	int dir = iIn/(abs(iIn));

	if(!m_bTabInit)
		SpeedTabInit();

	for(iOut = 0; iOut < 64; iOut++)
	{
		if(m_PanSpeedTab[iOut]>abs(iIn))
			break;
	}
	return (iOut)*dir;
}

int CPTZSpeedTransfer::GetTiltSpeed(int iIn)
{
	int iOut;
	if(iIn == 0)
		return 0;

	int dir = iIn/(abs(iIn));

	if(!m_bTabInit)
		SpeedTabInit();

	for(iOut = 0; iOut < 64; iOut++){
		if(m_TilSpeedTab[iOut]>abs(iIn))
			break;
	}
	return (iOut)*dir;
}

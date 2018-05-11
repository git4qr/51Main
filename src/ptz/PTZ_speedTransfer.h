#ifndef _PTZ_SPEED_TRANSFER_H
#define _PTZ_SPEED_TRANSFER_H

class CPTZSpeedTransfer 
{
public:
	CPTZSpeedTransfer();
	virtual ~CPTZSpeedTransfer();

	int m_PanSpeedTab[64];
	int m_TilSpeedTab[64];
	bool m_bTabInit;

public:
	int SpeedTabInit();
	int GetPanSpeed(int iIn);
	int GetTiltSpeed(int iIn);
};
#endif

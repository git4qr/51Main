#ifndef _APP_STATUS_H
#define _APP_STATUS_H

typedef enum {
	eSen_TV0	= 0x00,
	eSen_TV1	= 0x01,
	eSen_TV2   = 0x02,
	eSen_TV3 	= 0x03,
	eSen_FR1   = 0x04,
	eSen_Max   = 0x05
}eSenserStat;

typedef enum {
	Trk_Size0	= 0x00,
	Trk_Size1	= 0x01,
	Trk_Size2   = 0x02,
	Trk_SizeMax 	= 0x03
}eTrkAimSize;

#endif

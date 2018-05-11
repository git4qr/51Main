#ifndef _PLATFORMDEBUG_H_
#define _PLATFORMDEBUG_H_

#include <stdarg.h>

#define MAX_OUTPUT_STR_NUM 500
#define MAX_OUTPUT_STR_LEN 512

typedef struct
{
    int output_str_num;
	int output_str_rd;
	int output_str_wr;

    char output_str[MAX_OUTPUT_STR_NUM][MAX_OUTPUT_STR_LEN];
} PLATFORMDebug_obj;

void PlatformDebug_Init(PLATFORMDebug_obj *pObj);
int PlatformDebug_Insert(PLATFORMDebug_obj *pObj, char * str, int len);
int PlatformDebug_Flush(PLATFORMDebug_obj *pObj, char * str, int len);
int PlatformDebug_dump(const char *format, ...);

#endif //_PLATFORMDEBUG_H_

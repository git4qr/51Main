#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "platformDebug.h"

#ifndef min
#define min(a, b)	((a)<(b)?(a):(b))
#endif

PLATFORMDebug_obj * gOutputObj = NULL;
void PlatformDebug_Init(PLATFORMDebug_obj *pObj)
{
    memset(pObj, 0, sizeof(PLATFORMDebug_obj));
	gOutputObj = pObj;
}

int  PlatformDebug_Insert(PLATFORMDebug_obj *pObj, char * str, int len)
{
    if(pObj->output_str_num < MAX_OUTPUT_STR_NUM)
    {
        int index = pObj->output_str_wr;
        len = min(strlen(str), len);
        strncpy(pObj->output_str[index], str, len);

        pObj->output_str_num++;
        pObj->output_str_wr++;

        pObj->output_str_wr %= MAX_OUTPUT_STR_NUM;

        return 0;
    }

    return -1;
}

int PlatformDebug_Flush(PLATFORMDebug_obj *pObj, char * str, int len)
{
    int retval = 0;

    if(pObj->output_str_num > 0)
    {
        int index = pObj->output_str_rd;
        len = min(strlen(pObj->output_str[index]), len);
        strncpy(str, pObj->output_str[index],  len);

        pObj->output_str_num--;
        pObj->output_str_rd++;

        pObj->output_str_rd %= MAX_OUTPUT_STR_NUM;

        retval = pObj->output_str_num;
    }
    else
    {
        pObj->output_str_num = 0;
        retval = 0;
        strcpy(str, "no output string");
    }

    return retval;
}

int  PlatformDebug_dump(const char *format, ...)
{
    int retval = 0;
    char str[512];
	PLATFORMDebug_obj *pObj = gOutputObj;

    va_list args;
    va_start(args, format);
    //_vsnprintf(str, MAX_OUTPUT_STR_LEN, format, args);
    printf(str, MAX_OUTPUT_STR_LEN, format, args);
    va_end(args);

    PlatformDebug_Insert(pObj, str, strlen(str));
    return(retval);
}


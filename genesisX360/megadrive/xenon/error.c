/*
    error.c --
    Error logging 
*/

#include "shared.h"

FILE *error_log;

struct {
    int enabled;
    int verbose;
    FILE *log;
} t_error;

void error_init(void)
{
}

void error_shutdown(void)
{
}

void error(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	//if(error_log) vfprintf(error_log, format, ap);
	//char error_s[512];
	//vsprintf(error_s, format, ap);
	//fastPrintf(320,240,0x00000000,error_s);
	va_end(ap);
}


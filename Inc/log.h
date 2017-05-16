#ifndef __LOG_H__
#define __LOG_H__
#ifdef __cplusplus

class Log
{
public:
	static void Error(const char * Message);
	static void Waring(const char * Message);
	static void Infomation(const char * Message);
};

#endif //__cplusplus
#endif //__LOG_H__

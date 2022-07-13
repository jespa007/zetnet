#ifndef __ZN_LOG_H__
#define __ZN_LOG_H__
	


typedef enum{
	ZN_LOG_LEVEL_ERROR=0,
	ZN_LOG_LEVEL_WARNING,
	ZN_LOG_LEVEL_INFO,
	ZN_LOG_LEVEL_DEBUG
} ZNLogLevel;


#define ZN_LOG_ERROR(s, ...) 			zn_log_print(__FILE__,__LINE__,ZN_LOG_LEVEL_ERROR		, s, ##__VA_ARGS__)
#define ZN_LOG_ERRORF(s) 				ZN_LOG_ERROR(s, NULL)

#define ZN_LOG_WARNING(s, ...)   		zn_log_print(__FILE__,__LINE__,ZN_LOG_LEVEL_WARNING	, s, ##__VA_ARGS__)
#define ZN_LOG_WARNINGF(s)   			ZN_LOG_WARNING(s,NULL)

#define ZN_LOG_INFO(s, ...)   			ZN_LOG_PRINT(__FILE__,__LINE__,ZN_LOG_LEVEL_INFO		, s, ##__VA_ARGS__)
#define ZN_LOG_INFOF(s)   				ZN_LOG_INFO(s, NULL)

#ifdef __DEBUG__
#define __ZN_LOG_VERBOSE__
#endif

#ifdef __ZN_LOG_VERBOSE__
#define ZN_LOG_DEBUG(s, ...)   			zn_log_print(__FILE__,__LINE__,ZN_LOG_LEVEL_DEBUG		, s, ##__VA_ARGS__)
#else
#define ZN_LOG_DEBUG(s, ...)
#endif

#define ZN_LOG_DEBUGF(s)   				ZN_LOG_DEBUG(s,NULL)




void  zn_log_set_ansi_escape(bool _use);
void  zn_log_print(const  char  *file,  int  line, ZNLogLevel level, const  char  *string_text,...);




#endif
	

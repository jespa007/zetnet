#include "zetnet.h"

#define ZN_VAR_LOG(l) ((l) == ZN_LOG_LEVEL_ERROR?"ERR": (l)==ZN_LOG_LEVEL_WARNING?"WRN": (l) == ZN_LOG_LEVEL_INFO ? "INF" : (l) == ZN_LOG_LEVEL_DEBUG ? "DBG" : "NAN" )

	
static bool ansi_escape=true;

void  zn_log_set_ansi_escape(bool _use){
	ansi_escape=_use;
}

void  zn_log_print(const  char  *_file,  int  _line, ZNLogLevel _level, const  char  *_text_in,...) {

	FILE *std_type=stdout;
	char  text_out[MAX_PATH]={0};
	char filename[MAX_PATH]={"unknow"};
	size_t max_len=sizeof(text_out)/sizeof(char);
	va_list  ap;

	va_start(ap,  _text_in);
	vsnprintf(text_out,max_len,_text_in,  ap);




	switch(_level)
	{
		case ZN_LOG_LEVEL_WARNING:
	  case ZN_LOG_LEVEL_ERROR:
		std_type=stderr;
	break;
	  default:
	break;
	}

	if(_file != NULL){
		zn_path_get_file_name(filename,_file);
	}

	/*if(!ansi_escape){
		#ifdef _WIN32
		  int colors[7];
		  colors[0] = FOREGROUND_RED   | FOREGROUND_INTENSITY;//FOREGROUND_BLUE  | FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		  colors[1] = FOREGROUND_GREEN | FOREGROUND_RED   | FOREGROUND_INTENSITY;
		  colors[2] = FOREGROUND_BLUE  | FOREGROUND_GREEN | FOREGROUND_RED   | FOREGROUND_INTENSITY;
		  colors[3] = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		  colors[4] = FOREGROUND_BLUE  | FOREGROUND_INTENSITY;
		  colors[5] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		  colors[6] = FOREGROUND_RED   | FOREGROUND_INTENSITY;
		  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colors[level]);
		#endif
	}else{

		  switch(level)
		  {
			case LOG_LEVEL_ERROR:
				Term_SetColor(std_type,TERM_EFFECT_BRIGHT, TERM_COLOR_WHITE, TERM_COLOR_RED);
				break;
			case LOG_LEVEL_WARNING:
				Term_SetColor(std_type,TERM_EFFECT_BRIGHT,TERM_COLOR_WHITE, TERM_COLOR_YELLOW);
				break;
			default:

			break;
		  }
	}
*/
	if(_file == NULL)
		fprintf(std_type,"%s",text_out);
	else{
		fprintf(std_type,"[ %20s:%04i - %3s]=* %s",filename,_line,ZN_VAR_LOG(_level),text_out);
		//fprintf(stderr,"[ %30s:%04i - %3s]=%s",filename.c_str(),line,VAR_LOG(level),text);
	}

	/* if(!ansi_escape){
	#ifdef _WIN32
		// restore ...
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE  | FOREGROUND_GREEN | FOREGROUND_RED );
	#endif
	 }else{
	  Term_SetColor(std_type,TERM_EFFECT_BRIGHT, TERM_COLOR_WHITE, TERM_COLOR_BLACK);
	 }*/

    fprintf(std_type,"\n");


	  // to ensure all data has been write...
	  fflush(std_type);
}





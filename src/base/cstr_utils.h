#ifndef __ZN_CSTR_H__
#define __ZN_CSTR_H__


ZN_List * 	ZN_CStr_Split(const char * s, char delim);
bool 		ZN_CStr_EndsWith(const char * str, const char * ending);
void 		ZN_CStr_DeleteChar(char * str, char ch_to_remove);
char * 		ZN_CStr_FromInt(int number);
bool 		ZN_CStr_ToInt(int * i, const char *s, int base);
void 		ZN_CStr_ReplaceChar(char * str, char old_ch, char new_ch);


#endif

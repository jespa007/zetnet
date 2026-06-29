#ifndef __ZN_STRING_H__
#define __ZN_STRING_H__


ZN_List * 	ZN_String_Split(const char * s, char delim);
bool 		ZN_String_EndsWith(const char * str, const char * ending);
void 		ZN_String_DeleteChar(char * str, char ch_to_remove);
char * 		ZN_String_FromInt(int number);
bool 		ZN_String_ToInt(int * i, const char *s, int base);
void 		ZN_String_ReplaceChar(char * str, char old_ch, char new_ch);


#endif

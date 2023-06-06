#ifndef __ZN_STRING_H__
#define __ZN_STRING_H__


ZN_List * 	ZN_String_Split(const char * s, char delim);
bool 		ZN_String_EndsWith(const char * str, const char * ending);
void 		ZN_String_EraseChar(char * str, char ch_to_remove);
char * 		ZN_String_FromInt(int number);
bool 		ZN_String_ToInt(int * i, const char *s, int base);
void 		ZN_String_ReplaceByChar(char * str, char old_ch, char new_ch);
void 		ZN_String_Replace(char * str, const char * match_string, const char * replace_string);

#endif

#ifndef __ZNSTRING_H__
#define __ZNSTRING_H__


ZNList * 	ZNString_Split(const char * s, char delim);
bool 		ZNString_EndsWith(const char * str, const char * ending);
void 		ZNString_Remove(char * str, char ch_to_remove);
char * 		ZNString_IntToString(int number);
bool 		ZNString_StrToInt(int * i, const char *s, int base);
void 		ZNString_ReplaceChar(char * str, char old_ch, char new_ch);
void 		ZNString_ReplaceString(char * str, const char * match_string, const char * replace_string);

#endif

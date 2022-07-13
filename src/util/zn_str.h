#ifndef __ZN_STR_H__
#define __ZN_STR_H__


zn_list * 	zn_str_split(const char * s, char delim);
bool 		zn_str_ends_with(const char * str, const char * ending);
void 		zn_str_erase_char(char * str, char ch_to_remove);
char * 		zn_str_from_int(int number);
bool 		zn_str_to_int(int * i, const char *s, int base);
void 		zn_str_replace_by_char(char * str, char old_ch, char new_ch);
void 		zn_str_replace(char * str, const char * match_string, const char * replace_string);

#endif

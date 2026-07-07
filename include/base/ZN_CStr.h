#ifndef __ZN_CSTR_H__
#define __ZN_CSTR_H__


ZN_DLL_EXPORT char	*	ZN_CStr_New(const char *_str_in);
ZN_DLL_EXPORT char	*	ZN_CStr_NewLen(const char *_str_in, size_t _str_in_length);
ZN_DLL_EXPORT ZN_List * 	ZN_CStr_Split(const char * s, char delim);
ZN_DLL_EXPORT bool 		ZN_CStr_EndsWith(const char * str, const char * ending);
ZN_DLL_EXPORT void 		ZN_CStr_DeleteChar(char * str, char ch_to_remove);
ZN_DLL_EXPORT char * 		ZN_CStr_FromInt(int number);
ZN_DLL_EXPORT bool 		ZN_CStr_ToInt(int * i, const char *s, int base);
ZN_DLL_EXPORT void 		ZN_CStr_ReplaceChar(char * str, char old_ch, char new_ch);


#endif

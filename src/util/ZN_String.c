#include "zetnet.h"



void ZN_String_SplitBuiltIn(const char *s, char delim, ZN_List  * elems) {
	char delim_str[2]={delim,0};
	char * token = strtok((char *)s, delim_str);
   // loop through the string to extract all other tokens
   while( token != NULL ) {
	   size_t len=strlen(token)+1;
	   char *text=malloc(len+1);
	   strcpy(text,token);
	   ZN_List_Add(elems,text);
	   token = strtok(NULL, delim_str);
   }
}

ZN_List * ZN_String_Split(const char * s, char delim) {
	ZN_List *elems=ZN_List_New();
	if(s!=0 && strcmp(s,"")!=0){
		ZN_String_SplitBuiltIn(s, delim, elems);
	}
	return elems;
}

void  ZN_String_ReplaceChar(char * str, char old_ch, char new_ch){
	while(*str!=0){
		if(*str == old_ch){
			*str = new_ch;
		}
		str++;
	}
}

int ZN_String_Find(char *txt1,const char *txt2)
{
    char *posstr=strstr(txt1,txt2);
    if(posstr!=NULL)
    {
        return (posstr-txt1);
    }else
    {
        return -1;
    }
}

bool ZN_String_ToInt(int * i, const char *s, int base){
	 char *end;
	long  l;
	errno = 0;
	l = strtol(s, &end, base);
	if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
		fprintf(stderr,"\n\n\"%s\" number overflow",s);
		return false;
	}
	if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
		fprintf(stderr,"\n\n\"%s\" number underflow",s);
		return false;

	}
	if (*s == '\0' || *end != '\0') {
		fprintf(stderr,"\n\n\"%s\" number inconvertible",s);
		return false;
	}
	*i = l;
	return true;
}

char *ZN_String_dup(const char *in){
	size_t len=0;
	char *out;

	if(in==NULL) return NULL;

	len=strlen(in);

	out=malloc(len+1);
	strcpy(out,in);
	return out;

}

/*
void  ZN_String_Replace(char * str, const char * match_str, const char * replace_str){
	char *tmp=NULL;
	char *nextStr=NULL;
	int pos=0;

	tmp=ZN_String_dup(str);

	if(tmp == NULL) return;

	pos=ZN_String_Find(tmp,match_str);
	if(pos!=-1)
	{
		str[0]=0;
		strncpy(str,tmp,pos);
		str[pos]=0;

		strcat(str,replace_str);

		nextStr=tmp+pos+strlen(match_str);

		while(strlen(nextStr)!=0)
		{
			pos=ZN_String_Find(nextStr,match_str);

			if(pos==-1)
			{
				strcat(str,nextStr);
				break;
			}

			strncat(str,nextStr,pos);
			strcat(str,replace_str);

			nextStr=nextStr+pos+strlen(match_str);
		}
	}

	ZN_FREE(tmp);

}*/

void  ZN_String_DeleteChar(char * str, char ch_to_remove){

	while(*str!=0){
		if(*str == ch_to_remove){ // move 1 position left...
			char *aux=str;
			while(*(aux+1)!=0){
				*aux=*(aux+1);
				aux++;
			}
			*aux=0;
		}
		str++;
	}

}

char * ZN_String_FromInt(int number){
	static char to_number[256];
	sprintf(to_number,"%i",number);
	return to_number;
}

bool ZN_String_EndsWith(const char * str, const char * end_str){
	size_t len_str=strlen(str);
	size_t len_end_str=strlen(end_str);
	if(len_end_str<=len_str){
		return strcmp(str-len_end_str,end_str)==0;
	}

	return false;
}



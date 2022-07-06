#include "zetnet.h"



void zn_str_split_builtin(const char *s, char delim, zn_list  * elems) {
	char delim_str[2]={delim,0};
	char * token = strtok((char *)s, delim_str);
   // loop through the string to extract all other tokens
   while( token != NULL ) {
	   size_t len=strlen(token)+1;
	   char *text=malloc(len+1);
	   strcpy(text,token);
	   ZNList_Add(elems,text);
	   token = strtok(NULL, delim_str);
   }
}

zn_list * zn_str_split(const char * s, char delim) {
	zn_list *elems=ZNList_New();
	if(s!=0 && strcmp(s,"")!=0){
		zn_str_split_builtin(s, delim, elems);
	}
	return elems;
}

void  zn_str_replace_by_char(char * str, char old_ch, char new_ch){
	while(*str!=0){
		if(*str == old_ch){
			*str = new_ch;
		}
		str++;
	}
}

int zn_str_find(char *txt1,const char *txt2)
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

bool zn_str_to_int(int * i, const char *s, int base){
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

char *zn_str_dup(const char *in){
	size_t len=0;
	char *out;

	if(in==NULL) return NULL;

	len=strlen(in);

	out=malloc(len+1);
	strcpy(out,in);
	return out;

}

void  zn_str_replace(char * str, const char * match_str, const char * replace_str){
	char *tmp=NULL;
	char *nextStr=NULL;
	int pos=0;

	tmp=zn_str_dup(str);

	if(tmp == NULL) return;

	pos=zn_str_find(tmp,match_str);
	if(pos!=-1)
	{
		str[0]=0;
		strncpy(str,tmp,pos);
		str[pos]=0;

		strcat(str,replace_str);

		nextStr=tmp+pos+strlen(match_str);

		while(strlen(nextStr)!=0)
		{
			pos=zn_str_find(nextStr,match_str);

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

}

void  zn_str_erase_char(char * str, char ch_to_remove){

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


char * zn_str_from_int(int number){
	static char to_number[256];
	sprintf(to_number,"%i",number);
	return to_number;
}

bool zn_str_ends_with(const char * str, const char * end_str){
	size_t len_str=strlen(str);
	size_t len_end_str=strlen(end_str);
	if(len_end_str<=len_str){
		return strcmp(str-len_end_str,end_str)==0;
	}

	return false;
}



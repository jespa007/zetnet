#include "zetnet.h"

bool zn_file_exists(const char * filename) {

	if(filename == NULL) return false;

#ifdef _WIN32
	FILE *fp = fopen(filename,"r");

	if(fp == NULL) return false;

	fclose(fp);

	return true;
#else

	if(zn_dir_exists(filename)) return false;


	struct stat buffer;
	  return (stat (filename, &buffer) == 0);
#endif

}

uint8_t * zn_file_read(const char * filename,size_t * buffer_size){


	int  file_length, readed_elements;
	FILE  *fp;

	if((fp  =  fopen(filename,"rb"))  !=  NULL)
	{
		if((file_length = zn_file_length(filename)) != -1) {

			*buffer_size = file_length; // +1 is for string end

			uint8_t *buffer = (uint8_t *)malloc(*buffer_size);

			readed_elements = fread(buffer, 1, file_length, fp);

			if(readed_elements != file_length) {
				fprintf(stderr,"\nnumber elements doesn't match with length file (%s)\n",filename);
				ZN_FREE(buffer);
				return NULL;
			}

			fclose(fp);

			return buffer;
		}
		else  fprintf(stderr,"\nI can't read file \"%s\"\n",filename);
	}
	else  fprintf(stderr,"\nI can't open file \"%s\"\n",filename);


	return NULL;
}


int  zn_file_length(const char * filename)
{

	int  ini,  end;
	FILE  *fp;

	if((fp  =  fopen(filename,"rb"))  !=  NULL)
	{

		fseek(fp,  0,SEEK_SET);
		ini  =  ftell(fp);
		fseek(fp,  0,SEEK_END);
		end  =  ftell(fp);
		fclose(fp);

		return  (end  -  ini);
	}

	return    -1;
}

int  		ZNFile_LengthFromFile(FILE * file)
{

	int  ini,  end;
	FILE  *fp = file;

	if(fp != NULL)
	{
		long current = ftell(fp);

		fseek(fp,  0,SEEK_SET);
		ini  =  ftell(fp);
		fseek(fp,  0,SEEK_END);
		end  =  ftell(fp);

		fseek(fp,  current,SEEK_SET);
		return  (end  -  ini);
	}

	return    -1;
}

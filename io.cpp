#include "zetnet.h"

#ifndef MAX_PATH
#define MAX_PATH		4096
#endif


namespace zetnet{
	namespace io{

		bool file_exists(const std::string & m_file) {

			if(m_file == "") return false;

		#ifdef _WIN32
			FILE *fp = fopen(m_file.c_str(),"r");

			if(fp == NULL) return false;

			fclose(fp);

			return true;
		#else

			if(is_directory(m_file)) return false;


			struct stat buffer;
			  return (stat (m_file.c_str(), &buffer) == 0);
		#endif

		}

		uint8_t * read_file(const std::string & filename,uint32_t & size, bool end_string_char){


			int  file_length, readed_elements;
			FILE  *fp;
			int with_end_char=0;

			if(end_string_char){
				with_end_char=1;
			}

			if((fp  =  fopen(filename.c_str(),"rb"))  !=  NULL)
			{
				if((file_length = get_file_length(filename)) != -1) {

					size = file_length+with_end_char;

					uint8_t *buffer = (uint8_t *)malloc(size);
					memset(buffer,0,size );

					readed_elements = fread(buffer, 1, file_length, fp);

					if(readed_elements != file_length) {
						fprintf(stderr,"number elements doesn't match with length file (%s)\n",filename.c_str());
						free(buffer);
						return NULL;
					}

					fclose(fp);

					return buffer;
				}
				else  fprintf(stderr,"I can't read file \"%s\"\n",filename.c_str());
			}
			else  fprintf(stderr,"I can't open file \"%s\"\n",filename.c_str());


			return NULL;
		}


		int  get_file_length(const  std::string  & file)
		{

			int  ini,  end;
			FILE  *fp;

			if((fp  =  fopen(file.c_str(),"rb"))  !=  NULL)
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

		int  get_file_length(FILE  * file)
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

		bool set_work_path(const std::string & m_path) {

			if(chdir(m_path.c_str())==-1) {
				fprintf(stderr,"Cannot change working path to %s\n", m_path.c_str());
				return false;
			}
			return true;
		}

		std::string get_cwd(){
			char cwd[MAX_PATH];
		#ifdef __GNUC__
			if (getcwd(cwd, sizeof(cwd)) == NULL)
				 perror("getcwd() error");
		#else
			#ifdef _WIN32
			GetModuleFileName( NULL, cwd, MAX_PATH );
			#else
				#error "getCwd not supported!"
			#endif
		#endif

			std::string s_cwd = cwd;
			return s_cwd;
		}

		bool is_directory(const std::string & filename){
			int status;
			struct stat st_buf;
			//int ret_stat;

			status = stat (filename.c_str(), &st_buf);
			if (status != 0) {
				return false;
			}

			return S_ISDIR (st_buf.st_mode) != 0;
		}

		std::vector<std::string>  get_files(const std::string & folder, const std::string & filter, bool recursive){

			std::vector<std::string> list_file;
			std::vector<std::string> list_attribs = string::split(filter, '|');

			for(unsigned i = 0; i < list_attribs.size(); i++){
				list_attribs[i] = string::remove(list_attribs[i],' ');
				if(list_attribs[i] != "*")
					list_attribs[i] = string::remove(list_attribs[i],'*');
			}


			is_directory("/tmp");

			bool ok=false;
			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir (folder.c_str())) != NULL) {
			  while ((ent = readdir (dir)) != NULL) {

				  if((strcmp(ent->d_name,".")!=0) && (strcmp(ent->d_name,"..")!=0)){ // invalid dirs...


						  std::string data=folder+"/"+ent->d_name;
						  if(is_directory(data)){

								  if(recursive){
									  std::vector<std::string> r = get_files(data,filter);
									  list_file.insert(list_file.end(), r.begin(), r.end());
								  }
						  }
						  else{
							  ok=false;

							  for(unsigned i = 0; i < list_attribs.size() && !ok; i++){

								  if((list_attribs[i] == "*") || string::ends_with(ent->d_name,list_attribs[i])) {
									  list_file.push_back(data);
									  ok=true;
								  }
							  }
						  }
					  }
			  }
			  closedir (dir);
			} else {
				fprintf(stderr,"could not open directory %s\n",folder.c_str());
			}

			return list_file;
		}

	}
}

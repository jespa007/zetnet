
#include "HttpServer.h"


string  CIO_Utils::replace(const string & str_old, const char old_ch, char new_ch){
	string str = str_old;
	for (unsigned i = 0; i < str.length(); ++i) {
		if (str[i] == old_ch)
		  str[i] = new_ch;
	}

	return str;
}

void CIO_Utils::replace(string & str_input, const string & toReplace, const string & replaceWith){

	std::size_t found;;
	while((found = str_input.find(toReplace)) != std::string::npos){
		str_input = (str_input.replace(found, toReplace.length(), replaceWith));
	}


}

string  CIO_Utils::remove(string & str_old, char ch_to_remove){
	string str = str_old;
	string str_new="";

	for (unsigned i = 0; i < str_old.length(); ++i) {
		if (str_old[i] != ch_to_remove)
				str_new+=str_old[i];
	}

	return str_new;
}

vector<string> & CIO_Utils::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> CIO_Utils::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

string CIO_Utils::intToString(int number){

   std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

bool CIO_Utils::endsWith(const string & fullString, const string & ending){
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    }

    return false;
}


string  CIO_Utils::getDirectory(const string & _filename) {
	size_t found;
	string ss=".";
	found=_filename.find_last_of("/\\");

	if((int)found != -1)
		ss= _filename.substr(0,found);
	return ss;


}

string  CIO_Utils::getFileName(const string & _filename) {
  size_t found;
  string ss=_filename;

  //JEME_MEM_DISABLE_REGISTER_MEMORYLEAKS;

  found=_filename.find_last_of("/\\");
  if((int)found != -1)
	  ss= _filename.substr(found+1);

  //JEME_MEM_ENABLE_REGISTER_MEMORYLEAKS;
  return ss;
}

string  CIO_Utils::getFileNameWithoutExtension(const string & _path) {

	string file = getFileName(_path);


	std::string fName(file);
	    size_t pos = fName.rfind(".");
	    if(pos == std::string::npos)  //No extension.
	        return fName;

	    if(pos == 0)    //. is at the front. Not an extension.
	        return fName;

	    return fName.substr(0, pos);
}

//-----------------------------------------------------------------------------------------------v
// Normal file ops.




bool CIO_Utils::fileExists(const string & m_file) {

	if(m_file == "") return false;

#ifdef _WIN32
	FILE *fp = fopen(m_file.c_str(),"r");

	if(fp == NULL) return false;

	fclose(fp);

	return true;
#else

	if(isDirectory(m_file)) return false;


	struct stat buffer;
	  return (stat (m_file.c_str(), &buffer) == 0);
#endif


}


ByteBuffer * CIO_Utils::readFile(const string & filename, bool end_string_char){


	int  file_length, readed_elements;
	FILE  *fp;
	int with_end_char=0;

	if(end_string_char){
		with_end_char=1;
	}

	if((fp  =  fopen(filename.c_str(),"rb"))  !=  NULL)
	{
		if((file_length = getLength(filename)) != -1) {


			unsigned char *buffer = new unsigned char [file_length+with_end_char];
			memset(buffer,0,file_length+with_end_char );
			readed_elements = fread(buffer, 1, file_length, fp);

			if(readed_elements != file_length) {
				fprintf(stderr,"number elements doesn't match with length file (%s)\n",filename.c_str());
				delete  buffer;
				return NULL;
			}

			ByteBuffer *ch = new ByteBuffer(buffer, file_length+with_end_char);

			delete [] buffer;

			fclose(fp);

			return ch;
		}
		else  fprintf(stderr,"I can't read file \"%s\"\n",filename.c_str());
	}
	else  fprintf(stderr,"I can't open file \"%s\"\n",filename.c_str());


	return NULL;
}


int  CIO_Utils::getLength(const  string  & file)
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

int  CIO_Utils::getLength(FILE  * file)
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
		//fclose(fp);

		return  (end  -  ini);
	}

	return    -1;
}

bool CIO_Utils::setWorkPath(const string & m_path) {

	if(chdir(m_path.c_str())==-1) {
		fprintf(stderr,"Cannot change working path to %s\n", m_path.c_str());
		return false;
	}
	return true;
	//print_info_cr("%s", CIO_Utils::formatString("cd %s",m_path.c_str()));
	//system(CIO_Utils::formatString("cd %s",m_path.c_str()));
}

string CIO_Utils::getCwd(){
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

	string s_cwd = cwd;
	return s_cwd;
}




bool CIO_Utils::isDirectory(const string & filename){
	int status;
	struct stat st_buf;
	//int ret_stat;

	status = stat (filename.c_str(), &st_buf);
	    if (status != 0) {
	        //print_error_cr ("Error, errno = %d\n", errno);
	        return false;
	    }


	return S_ISDIR (st_buf.st_mode) != 0;


}



vector<string>  CIO_Utils::getFiles(const string & folder, const string & filter, bool recursive){



	vector<string> list_file;
	vector<string> list_attribs = CIO_Utils::split(filter, '|');

	for(unsigned i = 0; i < list_attribs.size(); i++){
		list_attribs[i] = CIO_Utils::remove(list_attribs[i],' ');
		if(list_attribs[i] != "*")
			list_attribs[i] = CIO_Utils::remove(list_attribs[i],'*');
	}


	isDirectory("/tmp");

	bool ok=false;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (folder.c_str())) != NULL) {
	  while ((ent = readdir (dir)) != NULL) {

		  if((strcmp(ent->d_name,".")!=0) && (strcmp(ent->d_name,"..")!=0)){ // invalid dirs...


			  	  string data=folder+"/"+ent->d_name;
				  if(isDirectory(data)){

					  	  if(recursive){
					  		  vector<string> r = getFiles(data,filter);
					  		  list_file.insert(list_file.end(), r.begin(), r.end());
					  	  }
				  }
				  else{
					  ok=false;

					  for(unsigned i = 0; i < list_attribs.size() && !ok; i++){

						  if((list_attribs[i] == "*") || CIO_Utils::endsWith(ent->d_name,list_attribs[i])) {
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


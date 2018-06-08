
#include "HttpServer.h"


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

time_t CIO_Utils::getModificationTime(const  string  & file){

	//struct tm* clock;				// create a time structure
	struct stat attrib;			// create a file attribute structure


	if(stat(file.c_str(), &attrib)!=-1)		// get the attributes of afile.txt
		return mktime(localtime(&(attrib.st_mtime)));	// Get the last modified time and put it into the time structure

	return 0; // failed!
}



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

bool CIO_Utils::writeFile(const string & filename, const string & data){


	FILE  *fp;
	bool ok=true;

	if((fp  =  fopen(filename.c_str(),"w"))  !=  NULL)
	{

			unsigned n = fwrite((char *)data.c_str(),1, data.size(),fp);

			if(n != data.size()) {
				fprintf(stderr,"number elements doesn't match with length file (%s)\n",filename.c_str());
				ok=false;
			}

			fclose(fp);



	}
	else  {
		fprintf(stderr,"I can't open file \"%s\"\n",filename.c_str());
		ok=false;
	}

	return ok;
}

bool CIO_Utils::writeFile(const string & filename, ByteBuffer * data){


	FILE  *fp;

	if(data==NULL)
		return false;

	if((fp  =  fopen(filename.c_str(),"wb"))  !=  NULL)
	{

			unsigned n = fwrite((char *)data->data_buffer,1, data->length,fp);

			if(n != data->length) {
				fprintf(stderr,"number elements doesn't match with length file (%s)\n",filename.c_str());
			}

			fclose(fp);

			return true;

	}
	else  fprintf(stderr,"I can't open file \"%s\"\n",filename.c_str());

	return false;
}

/*void CIO_Utils::getFilenameFromAbsolutePath(string & filename, const string & absolute_path_with_filename)
{
	char aux[500];
	STR_GetDirAndFileName(NULL,  aux,absolute_path_with_filename.c_str());

	filename = aux;
}*/

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
	//print_info_cr("%s", CStringUtils::formatString("cd %s",m_path.c_str()));
	//system(CStringUtils::formatString("cd %s",m_path.c_str()));
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

std::string CIO_Utils::getTempFile(){
	char tmp[512]={"/tmp"};
#ifdef _WIN32
	GetTempPath(sizeof(tmp),tmp);
#else
#endif

	return tmp;
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


bool CIO_Utils::createDirectory(const string & filename){


	if(filename == ""){
		fprintf(stderr,"empty entry\n");
		return false;
	}

	//if(!isDirectory(filename)){

		int status;
#ifdef _WIN32
		status = mkdir(filename.c_str());
#else
		status = mkdir(filename.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

		if(status == 0) return true;

		switch(status){
		case EACCES:
			fprintf(stderr,"Search permission is denied on a component of the path prefix, or write permission is denied on the parent directory of the directory to be created.\n");
			break;
		case EEXIST:
			fprintf(stderr,"The named file exists.\n");
			break;
		case ELOOP:
			fprintf(stderr,"A loop exists in symbolic links encountered during resolution of the path argument.\n");
			break;
		case EMLINK:
			fprintf(stderr,"The link count of the parent directory would exceed {LINK_MAX}.\n");
			break;
		case ENAMETOOLONG:
			fprintf(stderr,"The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}.\n");
			break;
		case ENOENT:
			fprintf(stderr,"A component of the path prefix specified by path does not name an existing directory or path is an empty string.\n");
			break;
		case ENOSPC:
			fprintf(stderr,"The file system does not contain enough space to hold the contents of the new directory or to extend the parent directory of the new directory.\n");
			break;
		case ENOTDIR:
			fprintf(stderr,"A component of the path prefix is not a directory.\n");
			break;
		case EROFS:
			fprintf(stderr,"The parent directory resides on a read-only file system.\n");
			break;

		}


	//}
	//else print_error_cr("directory already exist");

	return false;


}

vector<string>  CIO_Utils::getFiles(const string & folder, const string & filter, bool recursive){



	vector<string> list_file;
	vector<string> list_attribs = CStringUtils::split(filter, '|');

	for(unsigned i = 0; i < list_attribs.size(); i++){
		list_attribs[i] = CStringUtils::remove(list_attribs[i],' ');
		if(list_attribs[i] != "*")
			list_attribs[i] = CStringUtils::remove(list_attribs[i],'*');
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

						  if((list_attribs[i] == "*") || CStringUtils::endsWith(ent->d_name,list_attribs[i])) {
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


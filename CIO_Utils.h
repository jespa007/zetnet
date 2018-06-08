#pragma once



class CIO_Utils
{


	int m_totalLength;
	unsigned char *buffer;
	std::string  m_filename,m_flags;


public:
	static bool fileExists(const string & m_file);
	//static bool readFile(const string & filename, char *buffer, int length);
	static ByteBuffer * readFile(const string & filename, bool end_string_char = true);
	static bool writeFile(const string & filename, const string & data);
	static bool writeFile(const string & filename, ByteBuffer * data);
	static time_t getModificationTime(const  string  & file);
	static int  getLength(const  string  & file);
	static int  getLength(FILE * file);
	static bool setWorkPath(const string & m_path);
	static std::string getCwd();
	static std::string getTempFile();

	static std::string  getDirectory(const string & _filename);
	static std::string  getFileName(const string & _filename);
	static std::string  getFileNameWithoutExtension(const string & _filename);
	static bool	 isDirectory(const string & filename);
	static bool	 createDirectory(const string & filename);

	static std::vector<string > getFiles(const std::string & folder, const std::string & filter="*", bool recursive=false);
	//static void getFilenameFromAbsolutePath(string & filename, const string & absolute_path_with_filename);




};

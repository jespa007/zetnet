#pragma once

namespace zetnet{

class CZetNetUtils
{


	int m_totalLength;
	unsigned char *buffer;
	std::string  m_filename,m_flags;


public:

	static void delay(unsigned long _ms);


	static vector<string> &split(const string &s, char delim, vector<string> &elems);
	static vector<string> split(const string &s, char delim);
	static bool endsWith(const string & fullString, const string & ending);
	static string remove(string & str_old, char ch_to_remove);
	static string intToString(int number);
	static string replace(const string & str_old, const char old_ch, char new_ch);
	static void replace(string & str_input, const string & old_word, const string & new_word);


	static bool fileExists(const string & m_file);
	//static bool readFile(const string & filename, char *buffer, int length);
	static uint8_t * readFile(const string & filename, uint32_t & bytes_readed, bool end_string_char = true);
	static int  getLength(const  string  & file);
	static int  getLength(FILE * file);
	static bool setWorkPath(const string & m_path);
	static std::string getCwd();

	static std::string  getDirectory(const string & _filename);
	static std::string  getFileName(const string & _filename);
	static std::string  getFileNameWithoutExtension(const string & _filename);
	static bool	 isDirectory(const string & filename);

	static std::vector<string > getFiles(const std::string & folder, const std::string & filter="*", bool recursive=false);
	//static void getFilenameFromAbsolutePath(string & filename, const string & absolute_path_with_filename);




};
};

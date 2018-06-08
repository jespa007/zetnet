#pragma once

#define MAX_C_STRING 4096

// Util to capture args by ...
#define CAPTURE_VARIABLE_ARGS(text_out, text_in)\
{va_list  ap;\
va_start(ap,  text_in);\
int n=vsnprintf(text_out,MAX_C_STRING,text_in,  ap);\
if(n==4096){\
	text_out[sizeof(text_out)-2]='.';\
	text_out[sizeof(text_out)-3]='.';\
	text_out[sizeof(text_out)-4]='.';\
}\
va_end(ap);}


class CStringUtils{
	enum{
		MAX_STRING_BUFFERS=256,
		MAX_LENGTH_BUFFER=512
	};
	static char m_buffer[MAX_STRING_BUFFERS][MAX_LENGTH_BUFFER];
	static unsigned char m_index_buffer;

public:

	enum{
		STRING_IS_INVALID_NUMBER=0,
		STRING_IS_HEXA,
		STRING_IS_INT,
		STRING_IS_DOUBLE
	};

	template<typename T>
	static T fromString(const std::string& str){

	    std::istringstream ss(str);
	    T ret;
	    ss >> ret;
	    return ret;
	}

	static string intToString(int number);
	static string floatToString(float number);
	static string doubleToString(double number);
	static string toLower(const string & str);
	static string toUpper(const string & str);
	static std::wstring toWString(const std::string &s);
	static string formatString(const char *str, ...);
	static string hex2str(int hex);
	static int str2hex(const string & str_hex);

	static bool endsWith(const string & fullString, const string & ending);

	static vector<string> &split(const string &s, char delim, vector<string> &elems);
	static vector<string> split(const string &s, char delim);
	static bool isEmpty(const string & str);

	static bool isDigit(char c);
	static bool isHexaDigit(char c);
	static int isNumber(const string & test_number);

	static string replace(const string & str_old, const char old_ch, char new_ch);
	static void replace(string & str_input, const string & old_word, const string & new_word);

	static string remove(string & str_old, char ch_to_remove);

	static int count(const string & s,char c);

};

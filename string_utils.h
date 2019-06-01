#pragma once

namespace zetnet{

	namespace string_utils
	{

		 std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
		 std::vector<std::string> split(const std::string &s, char delim);
		 bool ends_with(const string & fullString, const std::string & ending);
		 std::string remove(string & str_old, char ch_to_remove);
		 std::string int_to_string(int number);
		 std::string replace(const string & str_old, const char old_ch, char new_ch);
		 void replace(string & str_input, const string & old_word, const std::string & new_word);

	}
}

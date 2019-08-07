#pragma once
#include <vector>
namespace zetnet{

	namespace string
	{

		 std::vector<std::string> split(const std::string &s, char delim);
		 bool ends_with(const std::string & fullString, const std::string & ending);
		 std::string remove(std::string & str_old, char ch_to_remove);
		 std::string to_string(int number);
		 std::string replace(const std::string & str_old, char old_ch, char new_ch);
		 std::string replace(const std::string & str_input, const std::string & old_word, const std::string & new_word);

	}
}

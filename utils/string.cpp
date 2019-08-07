
#include "zetnet.h"

namespace zetnet{
	namespace string{

		void split_builtin(const std::string &s, char delim, std::vector<std::string> &elems) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
		}

		std::vector<std::string> split(const std::string &s, char delim) {
			std::vector<std::string> elems;
			split_builtin(s, delim, elems);
			return elems;
		}

		std::string  replace(const std::string & str_input, char old_ch, char new_ch){
			std::string str_new = str_input;
			for (unsigned i = 0; i < str_input.length(); ++i) {
				if (str_new[i] == old_ch)
					str_new[i] = new_ch;
			}

			return str_new;
		}

		std::string  replace(const std::string & str_input, const std::string & old_str, const std::string & new_str){
			std::string str_replaced = str_input;
			std::size_t found;
			while((found = str_input.find(old_str)) != std::string::npos){
				str_replaced = (str_replaced.replace(found, old_str.length(), new_str));
			}
			return str_replaced;

		}

		std::string  remove(std::string & str_input, char ch_to_remove){
			std::string str = str_input;
			std::string str_new="";

			for (unsigned i = 0; i < str_input.length(); ++i) {
				if (str_input[i] != ch_to_remove)
						str_new+=str_input[i];
			}
			return str_new;
		}


		std::string to_string(int number){

		   std::stringstream ss;//create a stringstream
		   ss << number;//add number to the stream
		   return ss.str();//return a std::string with the contents of the stream
		}

		bool ends_with(const std::string & fullString, const std::string & ending){
			if (fullString.length() >= ending.length()) {
				return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
			}

			return false;
		}

	}

}

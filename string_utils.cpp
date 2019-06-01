
#include "zetnet.h"

namespace zetnet{
	namespace string_utils{



		std::string  replace(const std::string & str_old, const char old_ch, char new_ch){
			std::string str = str_old;
			for (unsigned i = 0; i < str.length(); ++i) {
				if (str[i] == old_ch)
				  str[i] = new_ch;
			}

			return str;
		}

		void replace(std::string & str_input, const std::string & toReplace, const std::string & replaceWith){

			std::size_t found;;
			while((found = str_input.find(toReplace)) != std::string::npos){
				str_input = (str_input.replace(found, toReplace.length(), replaceWith));
			}


		}

		std::string  remove(std::string & str_old, char ch_to_remove){
			std::string str = str_old;
			std::string str_new="";

			for (unsigned i = 0; i < str_old.length(); ++i) {
				if (str_old[i] != ch_to_remove)
						str_new+=str_old[i];
			}

			return str_new;
		}

		std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
			return elems;
		}


		std::vector<std::string> split(const std::string &s, char delim) {
			std::vector<std::string> elems;
			split(s, delim, elems);
			return elems;
		}

		std::string int_to_string(int number){

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




		//-----------------------------------------------------------------------------------------------v
		// Normal file ops.












	}

}

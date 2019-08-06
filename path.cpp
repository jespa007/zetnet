#include "zetnet.h"

namespace zetnet{
	namespace path{
		std::string  get_directory(const std::string & _filename) {
			size_t found;
			std::string ss=".";
			found=_filename.find_last_of("/\\");

			if((int)found != -1)
				ss= _filename.substr(0,found);
			return ss;


		}

		std::string  get_filename(const std::string & _filename) {
		  size_t found;
		  std::string ss=_filename;

		  //JEME_MEM_DISABLE_REGISTER_MEMORYLEAKS;

		  found=_filename.find_last_of("/\\");
		  if((int)found != -1)
			  ss= _filename.substr(found+1);

		  //JEME_MEM_ENABLE_REGISTER_MEMORYLEAKS;
		  return ss;
		}

		std::string  get_filename_without_extension(const std::string & _path) {

			std::string file = get_filename(_path);


			std::string fName(file);
				size_t pos = fName.rfind(".");
				if(pos == std::string::npos)  //No extension.
					return fName;

				if(pos == 0)    //. is at the front. Not an extension.
					return fName;

				return fName.substr(0, pos);
		}
	}
}

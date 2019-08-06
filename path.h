#pragma once

namespace zetnet{
	namespace path{

		std::string  get_directory(const std::string & _filename);
		std::string  get_filename(const std::string & _filename);
		std::string  get_filename_without_extension(const std::string & _filename);


	}
}

#pragma once

namespace zetnet{
	namespace io{
		 bool file_exists(const std::string & m_file);
		// bool readFile(const string & filename, char *buffer, int length);
		 uint8_t * read_file(const std::string & filename, uint32_t & bytes_readed, bool end_string_char = true);
		 int  get_file_length(const  std::string  & file);
		 int  get_file_length(FILE * file);
		 bool set_work_path(const std::string & m_path);
		 std::string get_cwd();

		 bool	 is_directory(const std::string & filename);

		 std::vector<std::string > get_files(const std::string & folder, const std::string & filter="*", bool recursive=false);
		// void getFilenameFromAbsolutePath(string & filename, const string & absolute_path_with_filename);

	}
}

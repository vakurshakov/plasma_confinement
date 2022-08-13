#include "./txt_file.hpp"

namespace fs = std::filesystem;


TXT_File::TXT_File(std::string directory_path, std::string file_name)
{
	fs::create_directories(fs::path(directory_path));
	txt_file_.open((directory_path + "/" + file_name + ".txt").c_str(),
		std::ios::out | std::ios::app);	
}

void TXT_File::write(double data)
{
	// txt_file_for_results_ <<  	
}

void TXT_File::flush()
{
	txt_file_.flush();
}

file_type TXT_File::get_type() { return file_type::txt; }
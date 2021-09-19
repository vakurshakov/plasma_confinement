#include "./bin_file.hpp"

#include <string>
#include <fstream>
#include <iostream> // debug: std::cout 
#include <filesystem>

namespace fs = std::filesystem;


BIN_File::BIN_File(std::string directory_path, std::string file_name)
{
	fs::create_directories(fs::path(directory_path));
	bin_file_.open((directory_path + "/" + file_name + ".bin").c_str(),
		std::ios::out | std::ios::binary);	
}

void BIN_File::write(double data)
{
	float fdata = static_cast<float>(data);
	bin_file_.write( (char*)&fdata, sizeof(float) );
}

file_type BIN_File::get_type() { return file_type::bin; }
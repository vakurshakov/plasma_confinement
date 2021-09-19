#ifndef FILE_WRITERS_BIN_FILE_HPP
#define FILE_WRITERS_BIN_FILE_HPP

//#################################################################################################

#include "./file_interface.hpp"

#include <fstream>
#include <string>


class BIN_File : public IFile {
public:
	BIN_File(std::string directory_path, std::string file_name);

	void write(double data) override;
	file_type get_type() override;

private:
	std::ofstream bin_file_;
};

//#################################################################################################

#endif // FILE_WRITERS_BIN_FILE_HPP
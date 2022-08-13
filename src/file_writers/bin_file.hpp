#ifndef FILE_WRITERS_BIN_FILE_HPP
#define FILE_WRITERS_BIN_FILE_HPP

//#################################################################################################

#include "./file_interface.hpp"

#include "src/pch.h"

class BIN_File : public IFile {
public:
	BIN_File(std::string directory_path, std::string file_name);

	void write(double data) override;

	virtual void flush() override;

	file_type get_type() override;

private:
	std::ofstream bin_file_;
};

//#################################################################################################

#endif // FILE_WRITERS_BIN_FILE_HPP
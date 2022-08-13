#ifndef FILE_WRITERS_TXT_FILE_HPP
#define FILE_WRITERS_TXT_FILE_HPP

//#################################################################################################

#include "./file_interface.hpp"

#include "src/pch.h"

class TXT_File : public IFile {
public:
	TXT_File(std::string directory_path, std::string file_name);
	
	void write(double data) override;

	virtual void flush() override;

	file_type get_type() override;

private:
	std::ofstream txt_file_;
};

//#################################################################################################

#endif // FILE_WRITERS_TXT_FILE_HPP
#ifndef FILE_WRITERS_FILE_INTERFACE_HPP
#define FILE_WRITERS_FILE_INTERFACE_HPP

//#################################################################################################

#include <fstream>
#include <string>

enum class file_type {
	txt, bin, hdf5
};


class IFile {
public:
	virtual void write(double data) = 0;
	virtual file_type get_type() = 0;
	virtual ~IFile() = default;
};


class HDF5_File : public IFile {
public:
	void write(double data) override;
	file_type get_type() override;
};

//#################################################################################################

#endif // FILE_WRITERS_FILE_INTERFACE_HPP

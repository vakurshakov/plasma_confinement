#ifndef FILE_WRITERS_FILE_INTERFACE_HPP
#define FILE_WRITERS_FILE_INTERFACE_HPP

#include "src/pch.h"

enum class file_type {
  bin
};

class IFile {
 public:
  virtual void write(double data) = 0;

  virtual void flush() = 0;

  virtual file_type get_type() = 0;

  virtual ~IFile() = default;
};

#endif // FILE_WRITERS_FILE_INTERFACE_HPP

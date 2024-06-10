#ifndef FILE_WRITERS_FILE_INTERFACE_HPP
#define FILE_WRITERS_FILE_INTERFACE_HPP

#include "src/pch.h"

class IFile {
 public:
  virtual ~IFile() = default;

  virtual void write(double data) = 0;
  virtual void flush() = 0;
  virtual void close() = 0;
};

#endif // FILE_WRITERS_FILE_INTERFACE_HPP

#ifndef FILE_WRITERS_BIN_FILE_HPP
#define FILE_WRITERS_BIN_FILE_HPP

#include "file_interface.hpp"

class BIN_File : public IFile {
  BIN_File() = default;

 public:
  BIN_File(std::string directory_path, std::string file_name);

  /// @param byte_offset The offset made from the last position after opening.
  /// @warning Should not be used for non-single-file diagnostics.
  static BIN_File
  from_backup(std::string directory_path, std::string file_name, int byte_offset);

  void write(double data) override;
  void write_double(double data);

  virtual void flush() override;

  file_type get_type() override;

 private:
  std::ofstream bin_file_;
};

#endif // FILE_WRITERS_BIN_FILE_HPP

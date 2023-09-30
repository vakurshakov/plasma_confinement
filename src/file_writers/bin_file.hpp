#ifndef FILE_WRITERS_BIN_FILE_HPP
#define FILE_WRITERS_BIN_FILE_HPP

#include "file_interface.hpp"

class BIN_File : public IFile {
  BIN_File() = default;

 public:
  BIN_File(std::string directory_path, std::string file_name);

  static BIN_File from_timestep(std::string directory_path, int t);

  /// @param byte_offset The offset made from the last position after opening.
  /// @warning Should not be used for non-single-file diagnostics.
  static BIN_File from_backup(std::string directory_path, std::string file_name, int byte_offset);

  void write(double data) override;
  void write_double(double data);

  void flush() override;
  void close() override;

 private:
  std::ofstream bin_file_;
};

#endif // FILE_WRITERS_BIN_FILE_HPP

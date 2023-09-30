#include "bin_file.hpp"

namespace fs = std::filesystem;


BIN_File::BIN_File(std::string directory_path, std::string file_name) {
  fs::create_directories(directory_path);
  bin_file_.open(directory_path + "/" + file_name + ".bin",
    std::ios::out | std::ios::trunc | std::ios::binary);
}

/* static */ BIN_File BIN_File::from_timestep(
    std::string directory_path, int t) {
  int time_width = std::to_string(TIME).size();

  std::stringstream ss;
  ss << std::setw(time_width) << std::setfill('0') << t;

  BIN_File result(directory_path, ss.str());

  return result;
}

/* static */ BIN_File BIN_File::from_backup(
    std::string directory_path, std::string file_name, int byte_offset) {
  fs::create_directories(directory_path);

  BIN_File result;

  result.bin_file_.open(directory_path + "/" + file_name + ".bin",
    std::ios::in | std::ios::out | std::ios::binary);

  result.bin_file_.seekp(-byte_offset, std::ios::end);

  return result;
}

void BIN_File::write(double data) {
  float _data = static_cast<float>(data);
  bin_file_.write(reinterpret_cast<char*>(&_data), sizeof(float));
}

void BIN_File::write_double(double data) {
  bin_file_.write(reinterpret_cast<char*>(&data), sizeof(double));
}

void BIN_File::flush() {
  bin_file_.flush();
}

void BIN_File::close() {
  flush();
  bin_file_.close();
}


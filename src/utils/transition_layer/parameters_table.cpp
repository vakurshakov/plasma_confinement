#include "parameters_table.hpp"

#include <iostream>

Parameters_table::Parameters_table(const std::string& filename) {
  evaluate_from_file(filename);
}

void Parameters_table::evaluate_from_file(const std::string& filename) {
  filename_ = filename;
  std::ifstream file(filename_);

  std::string name;
  double value;

  if (file.is_open()) {
    while (!file.eof()) {
      file >> name;
      file >> value;

      table_[name] = value;
    }
  }
}

void Parameters_table::rewrite() const {
  std::ofstream file(filename_);
  file.precision(15);

  for (const auto& [name, value] : table_) {
    file << name << " " << value << std::endl;
  }
  file << std::endl;
}

double& Parameters_table::operator[](const std::string& name) {
  return table_.operator[](name);
}

double Parameters_table::operator[](const std::string& name) const {
  return table_.at(name);
}

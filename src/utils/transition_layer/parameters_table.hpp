#ifndef PARAMETERS_TABLE_HPP
#define PARAMETERS_TABLE_HPP

#include "src/pch.h"

class Parameters_table {
 public:
  ~Parameters_table() = default;

  Parameters_table(const std::string& filename);
  void evaluate_from_file(const std::string& filename);

  double& operator[](const std::string& name);
  double operator[](const std::string& name) const;

  void rewrite() const;

  std::string filename_;

 private:
  Parameters_table() = delete;

  Parameters_table(const Parameters_table&) = delete;
  Parameters_table& operator=(const Parameters_table&) = delete;

  std::map<std::string, double> table_;
};


#endif  // PARAMETERS_TABLE_HPP

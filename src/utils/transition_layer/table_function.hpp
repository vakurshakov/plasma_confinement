#ifndef TABLE_FUNCTION_H
#define TABLE_FUNCTION_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

#include <string>

/**
 * @brief Carries the table-function values.
 *
 * @details It provides a table function that can be used via
 *  get_value method. The function gets the values from the .bin
 *  files provided by the Python scripts. Linear interpolation
 *  used for intermediate x values.
 */
class Table_function {
 public:
  ~Table_function();

  Table_function(const std::string& filename);
  void evaluate_from_file(const std::string& filename);

  /// @param x Coordinate to find a function value at (in c/w_pe units)
  /// @return Linearly interpolated value of a stored parameter function.
  double get_value(double x) const;

  // Minimal coordinate
  double get_x0() const { return x0_; }

  // Maximum coordinate
  double get_xmax() const { return xmax_; }

 private:
  Table_function() = delete;

  Table_function(const Table_function&) = delete;
  Table_function& operator=(const Table_function&) = delete;

  enum Pair { KEY = 0, VALUE = 1 };

  void clean_values();
  double linear_interpolation(double v0, double v1, double t) const;

  double x0_;    // [c/w_pe] - Start coordinate
  double xmax_;  // [c/w_pe] - Last coordinate
  double dx_;    // [c/w_pe] - Grid spacing

  int values_number_;
  double* values_ = NULL;
};


#endif  // Table_FUNCTION_HPP

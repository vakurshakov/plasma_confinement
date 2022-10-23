#ifndef PARAMETER_FUNCTION_HPP
#define PARAMETER_FUNCTION_HPP

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

namespace transition_layer {

/**
 * @brief Carries the parameter-function values.
 *
 * @details Combined with `evaluate_function.py`, it provides a table
 *  function that can be used via get_value method. The function gets
 *  the values from the .bin file provided by the Python script.
 *  Linear interpolation used for intermediate x values.
 *
 * @example
 *  Parameter_function foo("filename.bin");
 *  if (foo.get_x0() < x && x < foo.get_xmax()) {
 *      B(x) = B0 / sqrt(pi) * sqrt(acos(foo.get_value(x) - ...));
 *  }
 */
class Parameter_function {
 private:
  /// Only single instance of parameter function would be available.
  Parameter_function();

  /// Constraint so that two pointers do not point to the same memory.
  Parameter_function(const Parameter_function&) = delete;
  Parameter_function& operator=(const Parameter_function&) = delete;

  ~Parameter_function();

 public:
  static Parameter_function& instance() {
    static Parameter_function instance;
    return instance;
  }

  void evaluate_from_file(const char* filename);

  /// @param x Coordinate to find a function value at (in c/wp units)
  /// @return Linearly interpolated value of a stored parameter function.
  double get_value(double x) const;

  // Minimal coordinate where function equals to +1.
  double get_x0() const { return x0_; }

  // Maximum coordinate where function equals to -1.
  double get_xmax() const { return xmax_; }

 private:
  enum Pair { KEY = 0, VALUE = 1 };

  void clean_values();
  double linear_interpolation(double v0, double v1, double t) const;

  double x0_;    // [c/wp] - Start coordinate
  double xmax_;  // [c/wp] - Last coordinate
  double dx_;    // [c/wp] - Grid spacing

  double mi_;    // [me] - Mass of ions
  double me_;    // [me] - Mass of electrons

  int values_number_;
  double* values_ = NULL;
};

}  // namespace transition_layer

#endif  // PARAMETER_FUNCTION_HPP

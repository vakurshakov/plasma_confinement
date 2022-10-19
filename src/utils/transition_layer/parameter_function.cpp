#include "parameter_function.hpp"

#include "src/pch.h"

namespace transition_layer {

Parameter_function::Parameter_function() {
  this->evaluate_from_file(config::path_to_parameter_function);
}

/// Class is always carry some function table.
Parameter_function::~Parameter_function() {
  clean_values();
}

void Parameter_function::clean_values() {
  if (values_ != NULL) {
    free(values_);
  }
}

/// Reads parameters and function table from the file with
/// `filename`. Memory allocation happens, the previous
/// data will be freed. Data stored continuously, in pairs:
/// key-value.
void Parameter_function::evaluate_from_file(const char* filename) {
  clean_values();

  FILE* file = fopen(filename, "rb");
  if (!file) {
    perror((std::string("failed to open file: ") + filename).c_str());
    exit(EXIT_FAILURE);
  }

  struct stat sb;
  if (stat(filename, &sb) == -1) {
    perror((std::string("failed to read stat from file: ") + filename).c_str());
    exit(EXIT_FAILURE);
  }

  fread((char*) &x0_,   sizeof(double), 1, file);
  fread((char*) &xmax_, sizeof(double), 1, file);
  fread((char*) &dx_,   sizeof(double), 1, file);
  fread((char*) &mi_,   sizeof(double), 1, file);
  fread((char*) &me_,   sizeof(double), 1, file);

  int values_byte_size = sb.st_size - 5 * sizeof(double);
  values_number_ = values_byte_size / sizeof(double);

  values_ = (double*) malloc(values_byte_size);
  fread(values_, values_byte_size, 1, file);

  fclose(file);
}

/// Using linear interpolation it computes a value
/// of the stored parameter-function for a given x.
double Parameter_function::get_value(double x /*, c/wp */) const {
  assert((x0_ <= x && x <= xmax_) && "given x misses the boundaries");

  int pair_index = (int) round((x - x0_) / dx_);

  double answer = 0;

  if (0 <= pair_index && pair_index < values_number_ / 2 - 1) {
    double delta_x = x / dx_ - round(x / dx_);
    answer = this->linear_interpolation(
      values_[2 * pair_index + VALUE],
      values_[2 * (pair_index + 1) + VALUE],
      delta_x);
  }
  // Take into account the last pair with no following values.
  else if (pair_index == values_number_ / 2 - 1) {
    // Here x in (xmax-dx/2, xmax], so delta_x is negative.
    double delta_x = fabs(x / dx_ - round(x / dx_));
    answer = this->linear_interpolation(
      values_[2 * pair_index + VALUE],
      values_[2 * (pair_index - 1) + VALUE],
      delta_x);
  }

  // Check, that f(x) values is strictly between -1 and +1
  if (answer < -1.0) answer = -1.0;
  else if (answer > +1.0) answer = +1.0;

  return answer;
}


double Parameter_function::linear_interpolation(
    double v0, double v1, double t) const {
  return (1.0 - t) * v0 + t * v1;
}

}  // namespace transition_layer

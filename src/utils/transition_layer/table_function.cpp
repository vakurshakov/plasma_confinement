#include "table_function.hpp"

Table_function::Table_function(const std::string& filename) {
  evaluate_from_file(filename);
}

/// Class is always carry some function table.
Table_function::~Table_function() {
  clean_values();
}


/// Reads parameters and function table from the file with
/// `filename`. Memory allocation happens, the previous
/// data will be freed. Data stored continuously, in pairs:
/// key-value.
void Table_function::evaluate_from_file(const std::string& filename) {
  clean_values();

  FILE* file = fopen(filename.c_str(), "rb");
  if (!file) {
    perror((std::string("failed to open file: ") + filename).c_str());
    exit(EXIT_FAILURE);
  }

  struct stat sb;
  if (stat(filename.c_str(), &sb) == -1) {
    perror((std::string("failed to read stat from file: ") + filename).c_str());
    exit(EXIT_FAILURE);
  }

  fread((char*) &x0_,   sizeof(double), 1, file);
  fread((char*) &xmax_, sizeof(double), 1, file);
  fread((char*) &dx_,   sizeof(double), 1, file);

  int values_byte_size = sb.st_size - 3 * sizeof(double);
  values_number_ = values_byte_size / sizeof(double);

  values_ = (double*) malloc(values_byte_size);
  fread(values_, values_byte_size, 1, file);

  fclose(file);
}

double Table_function::operator()(double x /*, c/wp */) const {
  return get_value(x);
}

/// Using linear interpolation it computes a value
/// of the stored parameter-function for a given x.
double Table_function::get_value(double x /*, c/wp */) const {
  assert((get_x0() <= x && x <= get_xmax()) && "given x misses the boundaries");

  x -= get_x0();

  int pair_index = (int) round((x - x0_) / dx_);

  double answer = 0;

  if (0 <= pair_index && pair_index < values_number_ / 2 - 1) {
    double delta_x = x / dx_ - round(x / dx_);
    answer = linear_interpolation(
      values_[2 * pair_index + VALUE],
      values_[2 * (pair_index + 1) + VALUE],
      delta_x);
  }
  // Take into account the last pair with no following values.
  else if (pair_index == values_number_ / 2 - 1) {
    // Here x in (xmax-dx/2, xmax], so delta_x is negative.
    double delta_x = fabs(x / dx_ - round(x / dx_));
    answer = linear_interpolation(
      values_[2 * pair_index + VALUE],
      values_[2 * (pair_index - 1) + VALUE],
      delta_x);
  }

  return answer;
}


void Table_function::clean_values() {
  if (values_ != NULL) {
    free(values_);
  }
}

double Table_function::linear_interpolation(
    double v0, double v1, double t) const {
  return (1.0 - t) * v0 + t * v1;
}

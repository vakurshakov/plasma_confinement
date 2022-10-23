#include <iomanip>
#include <iostream>

#include "parameter_function.hpp"


int main() {
  static auto& foo = transition_layer::Parameter_function::instance();

  const double Omega_max = 0.5;

  std::cout << "Magnetic field in a transition layer: " << std::endl;

  for (double x = foo.get_x0(); x <= foo.get_xmax(); x += 0.05 /*, c/wp*/) {
    double fx = foo.get_value(x);

    double B = Omega_max / sqrt(M_PI) * sqrt(M_PI / 2. - asin(fx) - fx * sqrt(1 - fx * fx));

    std::cout << std::fixed << std::setprecision(3) << x
      << "\t" << std::setprecision(9) << fx << "\t" << B << std::endl;
  }

  return EXIT_SUCCESS;
}
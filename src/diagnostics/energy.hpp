#ifndef SRC_DIAGNOSTICS_ENERGY_HPP
#define SRC_DIAGNOSTICS_ENERGY_HPP

#include "diagnostic.hpp"
#include "src/particles/particles.hpp"
#include "src/vectors/vector3_field.hpp"

class energy_parameters_saver : public Diagnostic {
 protected:
  energy_parameters_saver(std::string result_directory);

  void save_parameters() const final;
};


class fields_energy : public energy_parameters_saver {
 public:
  fields_energy(std::string result_directory,
    const vector3_field& electric,
    const vector3_field& magnetic);

  void diagnose(int t) override;

 private:
  const vector3_field& electric_;
  const vector3_field& magnetic_;
};


class particles_energy : public energy_parameters_saver {
 public:
  particles_energy(
    std::string result_directory, std::string sort_name,
    const Particles& particles_sort);

  void diagnose(int t) override;

 private:
  const Particles& particles_;
};

#endif  // SRC_DIAGNOSTICS_ENERGY_HPP

#ifndef CMD_SET_FIELDS_DISTRIBUTION_HPP
#define CMD_SET_FIELDS_DISTRIBUTION_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/fields/fields.hpp"

class Set_fields_distribution final : public Command {
 public:
  Set_fields_distribution(
    Fields* fields,
    const vector3& electric_field,
    const vector3& magnetic_field);

  void execute(int /* timestep */) override;

  virtual bool needs_to_be_removed(int t) const override {
    return t > 0;
  }

private:
  Fields* fields_;
  vector3 electric_field_;
  vector3 magnetic_field_;
};

#endif  // CMD_SET_FIELDS_DISTRIBUTION_HPP

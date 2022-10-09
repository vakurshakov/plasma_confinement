#ifndef CMD_SET_BZ_DISTRIBUTION_HPP
#define CMD_SET_BZ_DISTRIBUTION_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/fields/fields.hpp"
#include "src/utils/transition_layer/parameter_function.hpp"

class Set_Bz_distribution : public Command {
 public:
  Set_Bz_distribution(Fields* const fields);

  void execute(int /* timestep */) const override;

  virtual bool needs_to_be_removed(int t) const override { return t > 0; }

private:
  Fields* const fields_;
};

#endif  // CMD_SET_BZ_DISTRIBUTION_HPP

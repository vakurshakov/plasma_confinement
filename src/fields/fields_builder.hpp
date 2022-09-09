#ifndef SRC_FIELDS_FIELDS_BUILDER_HPP
#define SRC_FIELDS_FIELDS_BUILDER_HPP

#include "src/pch.h"
#include "src/vectors/vector3_field.hpp"
#include "src/fields/open_boundaries_processor.hpp"
#include "src/diagnostics/diagnostics.hpp"

using std::map, std::multimap, std::string, std::vector;

class Fields_builder {
 public:
  Fields_builder() = default;

  std::unique_ptr<vector3_field>
  build_field(string type);

  Boundaries_processor_up
  build_boundary_processor(vector3_field& E, vector3_field& B);

	vector<std::unique_ptr<Fields_diagnostic>>
  build_diagnostics();
};

#endif  //SRC_FIELDS_FIELDS_BUILDER_HPP

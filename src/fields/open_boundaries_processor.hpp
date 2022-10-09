#ifndef SRC_FIELDS_OPEN_BOUNDARIES_PROCESSOR
#define SRC_FIELDS_OPEN_BOUNDARIES_PROCESSOR

/**
 * @brief Here is open boundary processor with its
 * configurative damping layer.
 *
 * In processor, simple layer were used: every cell
 * value inside this layer is multiplied by
 * coefficient < 1 depending on depth.
 */

#include "src/pch.h"
#include "src/vectors/vector3_field.hpp"

/// @todo Data about layer geometry is duplicated, remove it 
struct Damping_layer {
  int width;
  double damping_factor;
  double damping_coeff(int x);
};

class Open_boundaries_processor {
 public:
  Open_boundaries_processor(
    vector3_field& fields_E,
    vector3_field& fields_B,
    Damping_layer layer);

  void process();

 private:
  vector3_field& fields_E;
  vector3_field& fields_B;

  Damping_layer layer;

  void left_right_bounds();
  void top_bottom_bounds();
};

using Boundaries_processor_up = std::unique_ptr<Open_boundaries_processor>;

#endif  // SRC_FIELDS_OPEN_BOUNDARIES_PROCESSOR

#ifndef SRC_UTILS_DOMAIN_GEOMETRY_HPP
#define SRC_UTILS_DOMAIN_GEOMETRY_HPP

/**
 * @brief Small structure to represent the domain,
 * values should be passed in units of c/wp.
 *
 * @note The same structure should be used in both
 * cartesian and polar coordinates. Coordinates are
 * mapped: x to radius, and y to polar angle.
 */
struct Domain_geometry {
  double x_min = 0.0;
  double x_max = 0.0;
  double y_min = 0.0;
  double y_max = 0.0;

  Domain_geometry() = default;

  Domain_geometry(
    double x_min, double x_max,
    double y_min, double y_max)
    : x_min(x_min), x_max(x_max),
      y_min(y_min), y_max(y_max) {}
};

#endif  // SRC_UTILS_DOMAIN_GEOMETRY_HPP


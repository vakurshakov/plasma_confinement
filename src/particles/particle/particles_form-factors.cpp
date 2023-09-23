#include <cmath>

double zeroth_order_spline(double x, double mesh_grid)
{
  double abs_x = fabs(x / mesh_grid);

  if (abs_x <= 0.5) {
    return 1.0;
  }
  else { return 0; }
}


double first_order_spline(double x, double mesh_grid)
{
  double abs_x = fabs(x / mesh_grid);

  if (abs_x <= 1.0) {
    return 1.0 - abs_x;
  }
  else { return 0; }
}


double second_order_spline(double x, double mesh_grid)
{
  double abs_x = fabs(x / mesh_grid);

  if (abs_x <= 0.5) {
    return (0.75 - abs_x * abs_x);
  }
  else if (0.5 < abs_x && abs_x < 1.5) {
    return 0.5 * (1.5 - abs_x) * (1.5 - abs_x);
  }
  else { return 0; }
}


double third_order_spline(double x, double mesh_grid)
{
  double abs_x  = fabs(x / mesh_grid);
  double abs_x2 = abs_x * abs_x;
  double abs_x3 = abs_x * abs_x * abs_x;

  if (abs_x < 1) {
    return (4. - 6. * abs_x2 + 3. * abs_x3) / 6.;
  }
  else if (1 <= abs_x && abs_x < 2) {
    return (2. - abs_x) * (2. - abs_x) * (2. - abs_x) / 6.;
  }
  else { return 0; }
}

double fourth_order_spline(double x, double mesh_grid)
{
  double abs_x  = fabs(x / mesh_grid);
  double abs_x2 = abs_x * abs_x;
  double abs_x3 = abs_x * abs_x * abs_x;
  double abs_x4 = abs_x * abs_x * abs_x * abs_x;


  if (abs_x <= 0.5) {
    return (115. / 192. - 5. / 8. * abs_x2 + 1. / 4. * abs_x4);
  }
  else if (0.5 < abs_x && abs_x <= 1.5) {
    return (55. + 20. * abs_x - 120. * abs_x2 + 80. * abs_x3 - 16. * abs_x4) / 96.;
  }
  else if (1.5 < abs_x && abs_x < 2.5) {
    return (5. - 2. * abs_x) * (5. - 2. * abs_x) * (5. - 2. * abs_x) * (5. - 2. * abs_x) / 384.;
  }
  else { return 0; }
}

double fifth_order_spline(double x, double mesh_grid)
{
  double abs_x  = fabs(x / mesh_grid);
  double abs_x2 = abs_x * abs_x;
  double abs_x3 = abs_x * abs_x * abs_x;
  double abs_x4 = abs_x * abs_x * abs_x * abs_x;
  double abs_x5 = abs_x * abs_x * abs_x * abs_x * abs_x;

  if (abs_x <= 1) {
    return (11. / 20. - 0.5 * abs_x2 + 0.25 * abs_x4 - 1. / 12. * abs_x5);
  }
  else if (1 < abs_x && abs_x <= 2) {
    return (17. / 40. + 5. / 8. * abs_x - 7. / 4. * abs_x2 + 5. / 4. * abs_x3 - 3. / 8. * abs_x4 + 1. / 24. * abs_x5);
  }
  else if (2 < abs_x && abs_x < 3) {
    return (3. - abs_x) * (3. - abs_x) * (3. - abs_x) * (3. - abs_x) * (3. - abs_x) / 120.;
  }
  else { return 0; }
}

#include <omp.h>
#include <math.h>

double __0th_order_spline(double s, double ds) {
  double abs_s = fabs(s / ds);

  if (abs_s <= 0.5) {
    return 1.0;
  }
  else { return 0.0; }
}

double __1st_order_spline(double s, double ds) {
  double abs_s = fabs(s / ds);

  if (abs_s <= 1.0) {
    return 1.0 - abs_s;
  }
  else { return 0.0; }
}

double __2nd_order_spline(double s, double ds) {
  double abs_s = fabs(s / ds);

  if (abs_s <= 0.5) {
    return (0.75 - abs_s * abs_s);
  }
  else if (0.5 < abs_s && abs_s < 1.5) {
    return 0.5 * (1.5 - abs_s) * (1.5 - abs_s);
  }
  else { return 0.0; }
}

double __3rd_order_spline(double s, double ds) {
  double abs_s  = fabs(s / ds);
  double abs_x2 = abs_s * abs_s;
  double abs_x3 = abs_s * abs_s * abs_s;

  if (abs_s < 1.0) {
    return (4. - 6. * abs_x2 + 3. * abs_x3) / 6.;
  }
  else if (1.0 <= abs_s && abs_s < 2.0) {
    return (2. - abs_s) * (2. - abs_s) * (2. - abs_s) / 6.;
  }
  else { return 0.0; }
}

double __4th_order_spline(double s, double ds) {
  double abs_s  = fabs(s / ds);
  double abs_x2 = abs_s * abs_s;
  double abs_x3 = abs_s * abs_s * abs_s;
  double abs_x4 = abs_s * abs_s * abs_s * abs_s;

  if (abs_s <= 0.5) {
    return (115. / 192. - 5. / 8. * abs_x2 + 1. / 4. * abs_x4);
  }
  else if (0.5 < abs_s && abs_s <= 1.5) {
    return (55. + 20. * abs_s - 120. * abs_x2 + 80. * abs_x3 - 16. * abs_x4) / 96.;
  }
  else if (1.5 < abs_s && abs_s < 2.5) {
    return (5. - 2. * abs_s) * (5. - 2. * abs_s) * (5. - 2. * abs_s) * (5. - 2. * abs_s) / 384.;
  }
  else { return 0.0; }
}

double __5th_order_spline(double s, double ds) {
  double abs_s  = fabs(s / ds);
  double abs_x2 = abs_s * abs_s;
  double abs_x3 = abs_s * abs_s * abs_s;
  double abs_x4 = abs_s * abs_s * abs_s * abs_s;
  double abs_x5 = abs_s * abs_s * abs_s * abs_s * abs_s;

  if (abs_s <= 1.0) {
    return (11. / 20. - 0.5 * abs_x2 + 0.25 * abs_x4 - 1. / 12. * abs_x5);
  }
  else if (1.0 < abs_s && abs_s <= 2.0) {
    return (17. / 40. + 5. / 8. * abs_s - 7. / 4. * abs_x2 + 5. / 4. * abs_x3 - 3. / 8. * abs_x4 + 1. / 24. * abs_x5);
  }
  else if (2.0 < abs_s && abs_s < 3.0) {
    return (3. - abs_s) * (3. - abs_s) * (3. - abs_s) * (3. - abs_s) * (3. - abs_s) / 120.;
  }
  else { return 0.0; }
}

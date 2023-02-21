#include "Boris_pusher.hpp"

void Boris_pusher::process(Particle& particle, const vector3& local_E, const vector3& local_B) const
{
  // getting a useful variables from %parameters and %point
  const double alpha = 0.5 * dt * particle.q();
  const double m  = particle.m();
  vector2& r = particle.point.r;
  vector3& p = particle.point.p;

  const vector3 w = p + local_E * alpha;

  double energy = sqrt(m * m + w.dot(w));

  const vector3 h = local_B * alpha / energy;

  const vector3 s = h * 2. / (1. + h.dot(h));

  p = local_E * alpha + w * (1. - h.dot(s)) + w.cross(s) + h * (s.dot(w));

  energy = sqrt(m * m + p.dot(p));

  r += p.squeeze(Axes::XY) * dt / energy;
}

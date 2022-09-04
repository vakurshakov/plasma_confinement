#include "Boris_pusher.hpp"

namespace regular {

Boris_pusher::Boris_pusher(Particles2D3V& particles)
  : particles(particles) {}

void Boris_pusher::process(Particles::Particle_id i,
    const vector3& local_E, const vector3& local_B) const {
  double q0 = 0.5 * dt * particles.q(i);
  double m  = particles.m(i);

  // Realization of a Boris pusher
  vector3 w = particles.momentum[i] + q0 * local_E;

  double energy = sqrt(m * m + w.dot(w));

  vector3 h = q0 * local_B / energy;

  vector3 s = 2. * h / (1. + h.dot(h));

  vector3 new_p = q0 * local_E + w * (1. - h.dot(s)) + w.cross(s) + h * (s.dot(w));
  particles.momentum.px(i) = new_p.x;
  particles.momentum.py(i) = new_p.y;
  particles.momentum.pz(i) = new_p.z;

  energy = sqrt(m * m + new_p.dot(new_p));

  vector2 delta_x = new_p.project_to(vector3::Plane::XY) * dt / energy;
  particles.coordinate.x(i) += delta_x.x;  
  particles.coordinate.y(i) += delta_x.y;
}

}  // namespace regular

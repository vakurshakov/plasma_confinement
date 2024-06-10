#ifndef SRC_PARTICLES_PARTICLES_BOUNDARY_INTERACTION_HPP
#define SRC_PARTICLES_PARTICLES_BOUNDARY_INTERACTION_HPP

#include "src/pch.h"
#include "src/particles/particle/parameters.hpp"
#include "src/particles/particle/particle.hpp"
#include "src/vectors/vector_classes.hpp"
#include "src/utils/domain_geometry.hpp"

class Particles_boundary {
 public:
  Particles_boundary(std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom);

  virtual void add(Particle& particle, const vector2& r0) = 0;
  virtual void remove() = 0;

 protected:
  std::vector<Particle>& particles_vec_;
  Parameters& params_;
  Domain_geometry geom_;

  vector3 generate_moment(const Particle& particle);
};


class Plasma_boundary : public Particles_boundary {
 public:
  Plasma_boundary(std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom);

  void add(Particle& particle, const vector2& r0) override;
  void remove() override;

 private:
  inline bool passed_through_left(double x0, double ref_x) const;
  inline bool passed_through_right(double x0, double ref_x) const;
};


class Reflective_boundary : public Plasma_boundary {
 public:
  Reflective_boundary(std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom);

  void add(Particle& particle, const vector2& r0) override;
  void remove() override;
};


class Beam_boundary : public Plasma_boundary {
 public:
  Beam_boundary(std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom);

  void add(Particle& particle, const vector2& r0) override;
};


class Plasma_buffer : public Particles_boundary {
 public:
  Plasma_buffer(std::vector<Particle>& particles_vec, Parameters& params)
    : Particles_boundary(particles_vec, params, Domain_geometry()) {}

  void add(Particle& particle, const vector2& r0) override {}
  void remove() override { particles_vec_.clear(); }
};


class Beam_buffer : public Plasma_boundary {
 public:
  Beam_buffer(
    std::vector<Particle>& buff_beam_vec,
    std::vector<Particle>& main_beam_vec,
    Parameters& main_params,
    Domain_geometry geom);

  void add(Particle& particle, const vector2& r0) override;
  void remove() override;

 private:
  std::vector<Particle>& main_beam_vec_;
};


class POL_Beam_boundary : public Particles_boundary {
 public:
POL_Beam_boundary(std::vector<Particle>& particles_vec,
  Parameters& params, Domain_geometry geom);

  void add(Particle& particle, const vector2& r0) override {}
  void remove() override;
};

#endif  // SRC_PARTICLES_PARTICLES_BOUNDARY_INTERACTION_HPP

#ifndef DIAGNOSTICS_MARGINAL_DISTRIBUTION_HPP
#define DIAGNOSTICS_MARGINAL_DISTRIBUTION_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <cmath>
#include <string>
#include <vector>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../constants.h"


//-------------------------------------------------------------------------------------------------
class Projector {
public:
	Projector(const std::string& distribution_projection);
	virtual ~Projector() = default;

	const std::string& get_coordinates() const { return coordinates_; }
	virtual const double project_to_x(const Particle_parameters&, const Point&) const = 0;
	virtual const double project_to_y(const Particle_parameters&, const Point&) const = 0;

private:
	const std::string coordinates_;
};


class XY_projector : public Projector {
public:
	XY_projector() : Projector("density") {};
	const double project_to_x(const Particle_parameters&, const Point&) const override;
	const double project_to_y(const Particle_parameters&, const Point&) const override;
};


class VxVy_projector : public Projector {
public:
	VxVy_projector() : Projector("vy_on_vx") {};
	const double project_to_x(const Particle_parameters&, const Point&) const override;
	const double project_to_y(const Particle_parameters&, const Point&) const override;
};
//-------------------------------------------------------------------------------------------------


class marginal_distribution : public Particles_diagnostic {
public:
	marginal_distribution(std::string path,
		double projection_x_min, double projection_x_max, double dpx,
		double projection_y_min, double projection_y_max, double dpy,
		std::unique_ptr<Projector> projector);

	void save_parameters(std::string directory_path) override;
	void diagnose(const Particle_parameters&, const std::vector<Point>&, int t) override;

private:
	void collect(const Particle_parameters&, const std::vector<Point>&);
	void clear();

	double dpx_;
	double dpy_;
	int npx_min_;
	int npx_max_;
	int npy_min_;
	int npy_max_;
	
	std::vector<double> data_;
	std::unique_ptr<Projector> projector_;
	// std::unique_ptr<Value_to_> ;
};

//#################################################################################################

#endif // DIAGNOSTICS_MARGINAL_DISTRIBUTION_HPP
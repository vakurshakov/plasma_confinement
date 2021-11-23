#ifndef DIAGNOSTICS_DISTRIBUTION_MOMENT_HPP
#define DIAGNOSTICS_DISTRIBUTION_MOMENT_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "../particles/particle/concrete/particle_interface.hpp"
#include "../particles/particle/parameters/global_parameters.hpp"
#include "../vectors/vector_classes.hpp"
#include "../constants.h"

class Projector;
class Moment;
class distribution_moment;

//------- projector -------------------------------------------------------------------------------
class Projector {
public:
	Projector(const std::string distribution_projection) : axes_name_(distribution_projection) {};
	virtual ~Projector() = default;

	const std::string& get_axes_names() const { return axes_name_; }
	virtual const double project_to_x(const IParticle* const) const = 0;
	virtual const double project_to_y(const IParticle* const) const = 0;

private:
	const std::string axes_name_;
};


class XY_projector : public Projector {
public:
	XY_projector() : Projector("XY") {};

	const double project_to_x(const IParticle* const) const override;
	const double project_to_y(const IParticle* const) const override;
};


class VxVy_projector : public Projector {
public:
	VxVy_projector() : Projector("VxVy") {};

	const double project_to_x(const IParticle* const) const override;
	const double project_to_y(const IParticle* const) const override;
};

//------- moment ----------------------------------------------------------------------------------
class Moment {
public:
	Moment(const std::string distribution_projection) : moment_name_(distribution_projection) {};

	const std::string get_moment_name() { return moment_name_; }
	virtual const double get_quantity_to_be_averaged_(const IParticle* const) const = 0;

private:
	const std::string moment_name_;
};


class zeroth_moment : public Moment {
public:
	zeroth_moment() : Moment("Zeroth_moment") {};

	const double get_quantity_to_be_averaged_(const IParticle* const) const override;
};

class first_Vx_moment : public Moment {
public:
	first_Vx_moment() : Moment("Vx") {};

	const double get_quantity_to_be_averaged_(const IParticle* const) const override;
};


class first_Vy_moment : public Moment {
public:
	first_Vy_moment() : Moment("Vy") {};

	const double get_quantity_to_be_averaged_(const IParticle* const) const override;
};


//------- distribution_moment ---------------------------------------------------------------------
class distribution_moment : public Particles_diagnostic {
public:
	distribution_moment(std::string path,
		double projection_x_min, double projection_x_max, double dpx,
		double projection_y_min, double projection_y_max, double dpy,
		std::unique_ptr<Projector>,
		std::unique_ptr<Moment>			);

	void save_parameters(std::string directory_path) override;
	
	using particle_up = std::unique_ptr<IParticle>;
	void diagnose(const gParameters&, const std::vector<particle_up>&, int t) override;

private:
	void collect(const gParameters&, const std::vector<particle_up>&);
	void clear();

	double dpx_;
	double dpy_;
	int npx_min_;
	int npy_min_;
	int npx_max_;
	int npy_max_;
	
	std::vector<double> data_;
	std::unique_ptr<Projector> projector_;
	std::unique_ptr<Moment> moment_;
};

//#################################################################################################

#endif // DIAGNOSTICS_DISTRIBUTION_MOMENT_HPP
#ifndef MANAGERS_PARTICLES_HPP
#define MANAGERS_PARTICLES_HPP

//#################################################################################################

#include "../diagnostics/diagnostics.hpp"
#include "../particle/point.hpp"
#include "../particle/particle_parameters.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"

#include <memory>
#include <functional>
#include <forward_list>


using std::function;
using v3f = vector3_field;
using diagnostic_up = std::unique_ptr<Diagnostic>;


class Particles {
public:
	Particles() = default;
	Particles(
		Particle_parameters, std::vector<Point>,
		function<void(const Particle_parameters&, Point&,
			const v3f& E, const v3f& B)>,
		function<void(const Particle_parameters&, const Point&,
			const vector2& r0, v3f& j)>,
		function<void(Point&, double)> x_boundary,
		function<void(Point&, double)> y_boundary,
		std::forward_list<diagnostic_up>& diagnostics 	);


	// main Particles methods
	void particle_push(int i, const v3f& E, const v3f& B); 
	void density_decomposition(int i, const vector2& r0, v3f& j);
	void boundaries_processing(int i, double size_x, double size_y);
	void diagnose(int t);


	Point& element(int i) { return points_[i]; };
	Point element(int i) const { return points_[i]; };

	bool empty();

protected:
	// main Particles fields
	Particle_parameters parameters_;
	std::vector<Point> points_;

	// solvers for particles
	function<void(const Particle_parameters&, Point&,
		const v3f& E, const v3f& B)> particle_push_ = nullptr;
	function<void(const Particle_parameters&, const Point&,
		const vector2& r0, v3f& j)> density_decomposition_ = nullptr;
	function<void(Point&, double size_x, double size_y)> boundaries_processing_ = nullptr;

	// used in function<%> boundaries_processing
	function<void(Point&, double)> x_boundary_ = nullptr;
	function<void(Point&, double)> y_boundary_ = nullptr;
	
	// list of diagnostics for Particles
	std::forward_list<diagnostic_up> diagnostics_; 
};


class Electrons : public Particles {
public:
	Electrons(
		Particle_parameters params, std::vector<Point> points,
		function<void(const Particle_parameters&, Point&,
			const v3f& E, const v3f& B)> particle_push,
		function<void(const Particle_parameters&, const Point&,
			const vector2& r0, v3f& j)> density_decomposition,
		function<void(Point&, double)> x_boundary,
		function<void(Point&, double)> y_boundary,
		std::forward_list<diagnostic_up> diagnostics)
			: Particles(params, points, particle_push,
				density_decomposition, x_boundary, y_boundary, diagnostics) {};
};


class Protons : public Particles {
public: 
	Protons(
		Particle_parameters params, std::vector<Point> points,
		function<void(const Particle_parameters&, Point&,
			const v3f& E, const v3f& B)> particle_push,
		function<void(const Particle_parameters&, const Point&,
			const vector2& r0, v3f& j)> density_decomposition,
		function<void(Point&, double)> x_boundary,
		function<void(Point&, double)> y_boundary,
		std::forward_list<diagnostic_up> diagnostics)
			: Particles(params, points, particle_push,
				density_decomposition, x_boundary, y_boundary, diagnostics) {};
	
	//void add_ion_density(int t);
};

class Ions : public Particles {
public: 
	Ions(
		Particle_parameters params, std::vector<Point> points,
		function<void(const Particle_parameters&, Point&,
			const v3f& E, const v3f& B)> particle_push,
		function<void(const Particle_parameters&, const Point&,
			const vector2& r0, v3f& j)> density_decomposition,
		function<void(Point&, double)> x_boundary,
		function<void(Point&, double)> y_boundary,
		std::forward_list<diagnostic_up> diagnostics)
			: Particles(params, points, particle_push,
				density_decomposition, x_boundary, y_boundary, diagnostics) {};

	void add_ion_density(int t);
};
//#################################################################################################

#endif // MANAGERS_PARTICLES_HPP

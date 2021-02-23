#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "../vectors/vector_classes.hpp"
#endif

#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "../vectors/vector3_field.hpp"
#endif

#ifndef PARTICLES_H
#define PARTICLES_H
	#include "../particles/class_particles.hpp"
#endif

#include <memory>
#include <functional>


//######## written solvers #############################################################

	void Boris_pusher(const class_particles& SORT, particle& PARTICLE,
					  const vector3_field& E, const vector3_field& B);
	
	void Esirkepov_density_decomposition(const class_particles& SORT,
										 const particle& PARTICLE,
										 const vector2& r0,
										 vector3_field& J);
	
	void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& j);
	
	void add_circular_current(vector3_field& j, class_particles& sort, double v_inj, double Bz);

//######################################################################################

using namespace std;
using up_v3f = unique_ptr<vector3_field>;

class solvers_manager {
private:
	function<void(particle&, double)> x_boundary;
	function<void(particle&, double)> y_boundary;

	function<void(const class_particles&, particle&, const vector3_field& E, const vector3_field& B)> Particle_push_;
	function<void(const class_particles&, const particle&, const vector2& r0, vector3_field& j)> Density_decomposition_;
	function<void(particle&, double size_x, double size_y)> Boundaries_processing_;
	function<void(vector3_field& E, vector3_field& B, vector3_field& j)> Propogate_fields_;

	// @ с этим я не знаю как бороться в рамках моего построения.
	// NOTE: всякие таккие штуки можно собирать в отдельный vector<function<...>>

	//function<void(particle&, double)>	circular_current_init(j, ni, vi, mi, d);

public:
	solvers_manager() = default;
	void initialisation(up_v3f& E, up_v3f& B, up_v3f& j, vector<class_particles> particles,
		vector<string> solvers, vector<string> configuration);

	void Particle_push(const class_particles&, particle&, const up_v3f& E, const up_v3f& B); 
	void Density_decomposition(const class_particles&, const particle&, const vector2& r0, up_v3f& j);
	void Boundaries_processing(particle&, double size_x, double size_y);
	void Propogate_fields(up_v3f& E, up_v3f& B, up_v3f& j);
};
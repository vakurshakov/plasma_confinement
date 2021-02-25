#include "../solvers/solvers.hpp"
#include "../vectors/vector3_field.hpp"
#include "../particles/class_particles.hpp"
#include "../constants.h"

#include <memory>
#include <functional>


using namespace std;
using up_v3f = unique_ptr<vector3_field>;

#ifndef FIELDS_MANAGER_H
#define FIELDS_MANAGER_H

//#################################################################################################

class Fields_manager {
private:
	up_v3f E, B, j;

	function<void(vector3_field& E, vector3_field& B, vector3_field& j)> Propogate_fields_;

public:
	Fields_manager() = default;
	void initialisation(string field_solver, vector<string> field_configuration);

	void Propogate_fields();
	void add_circular_current(const class_particles& sort, double v_inj_, double Bz0_, int t);
	void add_Bz0(double Bz0_);
};

//#################################################################################################

#endif

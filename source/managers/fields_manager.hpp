#include "../solvers/solvers.hpp"
#include "../vectors/vector3_field.hpp"
#include "../particles/class_particles.hpp"
#include "../diagnostics/diagnostics.hpp"
#include "../constants.h"

#include <memory>
#include <functional>


using namespace std;
using v3f_up = unique_ptr<vector3_field>;
using diagnostic_up = unique_ptr<diagnostic>;

#ifndef FIELDS_MANAGER_H
#define FIELDS_MANAGER_H

//#################################################################################################

class Fields_manager {
private:
	v3f_up E_, B_, j_;

	vector<diagnostic_up> diagnostics; 

	function<void(vector3_field& E, vector3_field& B, vector3_field& j)> Propogate_fields_;

public:
	Fields_manager() = default;
	void initialization(string solver, vector<string> configuration,
		string test_name, map<string, vector<string>> diagnostics);

	void Propogate_fields();
	void diagnose();

	void add_circular_current(const class_particles& sort, double v_inj_, double Bz0_, int t);
	void add_Bz0(double Bz0_);
};

//#################################################################################################

#endif

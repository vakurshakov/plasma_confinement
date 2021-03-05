#ifndef FIELDS_H
#define FIELDS_H

//#################################################################################################

#include "../solvers/solvers.hpp"
#include "../vectors/vector3_field.hpp"
#include "../particles/species_description.hpp"
#include "../diagnostics/diagnostics.hpp"
#include "../constants.h"

#include <memory>
#include <functional>


using namespace std;
using v3f_up = unique_ptr<vector3_field>;
using diagnostic_up = unique_ptr<Diagnostic>;


class Fields {
public:
	Fields() = default;
	void initialize(string solver, vector<string> configuration,
		string test_name, multimap<string, vector<string>> diagnostics_description);

	// getters
	vector3_field& E() { return *E_; }
	vector3_field& B() { return *B_; }
	vector3_field& j() { return *j_; }

	// main Field's methods
	void propogate();
	void diagnose();

	// additional
	void add_circular_current(int t);
	void add_Bz0(double Bz0_);

private:
	v3f_up E_, B_, j_;

	vector<diagnostic_up> diagnostics_; 

	function<void(vector3_field& E, vector3_field& B, vector3_field& j)> propogate_;
};

//#################################################################################################

#endif

#include "./fields.hpp" 


enum CONF { SX, SY, BOUND };
enum DESC { FIELD, AXIS, PX, PY };

void Fields::initialization(string solver, vector<string> configuration,
	string test_name, map<string, vector<string>> diagnostics_description)
{
	// initalization of solver
	if ( solver == "FDTD_2D" ) {
		propogate_ = FDTD_2D;			
	}


	// initalization of configuration
	int size_x = stoi(configuration[SX]);
	int size_y = stoi(configuration[SY]);
	string boundaries = configuration[BOUND];
	
	using ref__v3f = reflective_vector3_field;
	using per__v3f = periodic_vector3_field;
	using rhpv_v3f = rh_pv_vector3_field;
	using rvph_v3f = rv_ph_vector3_field;
	
	if ( boundaries == "reflective" ) {
		E_ = make_unique<ref__v3f>(size_x, size_y);
		B_ = make_unique<ref__v3f>(size_x, size_y);
		j_ = make_unique<ref__v3f>(size_x, size_y);
	}
	else if ( boundaries == "periodic" ) {
		E_ = make_unique<per__v3f>(size_x, size_y);
		B_ = make_unique<per__v3f>(size_x, size_y);
		j_ = make_unique<per__v3f>(size_x, size_y);
	}
	else if ( boundaries == "rh_pv" ) {
		E_ = make_unique<rhpv_v3f>(size_x, size_y);
		B_ = make_unique<rhpv_v3f>(size_x, size_y);
		j_ = make_unique<rhpv_v3f>(size_x, size_y);
	}
	else if ( boundaries == "rv_ph" ) {
		E_ = make_unique<rvph_v3f>(size_x, size_y);
		B_ = make_unique<rvph_v3f>(size_x, size_y);
		j_ = make_unique<rvph_v3f>(size_x, size_y);
	}


	// initalization of diagnostics
	for (auto& diagnostic : diagnostics_description) {
		if ( diagnostic.first == "energy" ) {
			diagnostics.emplace_back(make_unique<fields_energy>(test_name + "/"
				+ diagnostic.first));
		}
		else if ( diagnostic.first == "whole_field" ) {
			diagnostics.emplace_back(make_unique<whole_field>(test_name + "/"
				+ diagnostic.first, diagnostic.second[DESC::FIELD], 
									stoi(diagnostic.second[DESC::AXIS]) ) );
		}
		else if ( diagnostic.first == "field_at_point" ) {
			diagnostics.emplace_back(make_unique<field_at_point>(test_name + "/"
				+ diagnostic.first, diagnostic.second[DESC::FIELD],
									stoi(diagnostic.second[DESC::AXIS]), 
									stoi(diagnostic.second[DESC::PX]),
									stoi(diagnostic.second[DESC::PY]) ) );
		}
	}
	
	diagnostics.shrink_to_fit();
}

void Fields::propogate()
{
	propogate_(*E_, *B_, *j_);
}

void Fields::diagnose()
{
	for (auto& diagnostic : diagnostics) {
		(*diagnostic).diagnose(*E_, *B_, *j_);
	}
}
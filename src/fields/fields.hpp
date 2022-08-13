#ifndef MANAGERS_FIELDS_HPP
#define MANAGERS_FIELDS_HPP

//#################################################################################################

#include "src/pch.h"
#include "../diagnostics/diagnostics.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


class Fields {
public:
	Fields() = default;

	using v3f = vector3_field;
	using v3f_up = std::unique_ptr<v3f>;
	using diagnostic_up = std::unique_ptr<Fields_diagnostic>;
	
	Fields( v3f_up&& E, v3f_up&& B, v3f_up&& J,
			std::function<void(v3f& E, v3f& B, v3f& J)>&& propogator,
			std::vector<diagnostic_up>&& diagnostics 		);
	
	// getters
	v3f& E() { return *E_; };
	v3f& B() { return *B_; };
	v3f& J() { return *J_; };

	// main Field methods
	void propogate();
	void diagnose(int t) const;

	// additional
	friend class Magnetic_field_half_step;
	void add_Bz0(double Bz0);
	void add_ion_current(int t);

private:
	v3f_up E_, B_, J_;

	std::function<void(v3f& E, v3f& B, v3f& J)> propogate_ = nullptr;
	
	std::vector<diagnostic_up> diagnostics_;
};

//#################################################################################################

#endif // MANAGERS_FIELDS_HPP

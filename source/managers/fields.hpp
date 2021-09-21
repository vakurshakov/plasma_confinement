#ifndef MANAGERS_FIELDS_HPP
#define MANAGERS_FIELDS_HPP

//#################################################################################################

//#include "../command.hpp"
//#include "../Add_Bz0.hpp"

#include "../diagnostics/diagnostics.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"

#include <forward_list>
#include <functional>
#include <memory>


using std::function, std::forward_list;
using v3f = vector3_field;
using v3f_up = std::unique_ptr<vector3_field>;
//using command_up = std::unique_ptr<Command>; 
using diagnostic_up = std::unique_ptr<Diagnostic>;


class Fields {
public:
	Fields() = default;
	Fields( v3f_up&& E, v3f_up&& B, v3f_up&& J,
			function<void(v3f& E, v3f& B, v3f& J)> propogator,
			forward_list<diagnostic_up>&& diagnostics 		);
	
	// getters
	v3f& E() { return *E_; }
	v3f& B() { return *B_; }
	v3f& J() { return *J_; }

	// main Field methods
	void propogate();
	void diagnose(int t);

	bool empty();

	// additional
	void add_Bz0(double Bz0);
	void add_ion_current(int t);

private:
	v3f_up E_, B_, J_;

	function<void(v3f& E, v3f& B, v3f& J)> propogate_ = nullptr;
	
	forward_list<diagnostic_up> diagnostics_;
};

//#################################################################################################

#endif // MANAGERS_FIELDS_HPP

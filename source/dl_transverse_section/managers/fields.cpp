#include "./fields.hpp"

#include <memory>

#include "../diagnostics/diagnostics.hpp"
#include "../vectors/vector3_field.hpp"


using v3f = vector3_field;
using v3f_up = unique_ptr<vector3_field>;


Fields::Fields(	v3f_up E, v3f_up B, v3f_up J,
				function<void(v3f& E, v3f& B, v3f& J)> propogate,
				forward_list<diagnostic_up> diagnostics)
{
	E_.swap(E);
	B_.swap(B);
	J_.swap(J);
	propogate_ = propogate;
	diagnostics_.swap(diagnostics);
}


void Fields::propogate()
{
	propogate_(*E_, *B_, *J_);
}


void Fields::diagnose(int t)
{
	if  (!diagnostics_.empty()) {
		for (auto& diagnostic : diagnostics_) {
			diagnostic->diagnose(*E_, *B_, *J_, t);
		}
	}
}
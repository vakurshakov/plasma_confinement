#include "./fields.hpp"

#include "../diagnostics/diagnostics.hpp"
#include "../vectors/vector3_field.hpp"


using v3f = vector3_field;
using v3f_up = std::unique_ptr<v3f>;


Fields::Fields(	v3f_up&& E, v3f_up&& B, v3f_up&& J,
				std::function<void(v3f& E, v3f& B, v3f& J)>&& propogate,
				std::vector<diagnostic_up>&& diagnostics)
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


void Fields::diagnose(int t) const
{
	#pragma omp parallel for shared(diagnostics_), num_threads(THREAD_NUM)
	for (auto& diagnostic : diagnostics_) {
		diagnostic->diagnose(*E_, *B_, *J_, t);
	}
}
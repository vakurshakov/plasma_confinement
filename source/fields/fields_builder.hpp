#ifndef FIELDS_BUILDER_HPP
#define FIELDS_BUILDER_HPP

//#################################################################################################

#include "fields.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "../particles/particles.hpp"


using std::map, std::multimap, std::string, std::vector;
using v3f = vector3_field;
using v3f_up = std::unique_ptr<vector3_field>;
using diagnostic_up = std::unique_ptr<Diagnostic>;


class Fields_builder {
public:
	Fields_builder() = default;
	Fields build();
	
private:
	// Методы для постороения полей
	function<void(v3f& E, v3f& B, v3f& j)> propogator();
	v3f_up load_field(string type);


	// Методы для построения частиц
	Particle_parameters config_parameters(vector<string> parameters);
	vector<Point> load_particles(vector<string> distribution);
	
	auto particle_push(vector<string> description);
	auto density_decomposition(vector<string> description);
	auto x_boundary();
	auto y_boundary();

	// Возвращает список диагностик
	forward_list<diagnostic_up> diagnostics_list(multimap<string, vector<string>> description);
};


//#################################################################################################

#endif //FIELDS_BUILDER_HPP
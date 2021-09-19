#ifndef ENTITIES_BUILDER_H
#define ENTITIES_BUILDER_H

//#################################################################################################

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "./fields.hpp"
#include "./particles.hpp"


using std::map, std::multimap, std::string, std::vector;
using v3f = vector3_field;
using v3f_up = unique_ptr<vector3_field>;
using diagnostic_up = unique_ptr<Diagnostic>;


class Entities_builder {
public:
	Entities_builder() = default;
	
	void save_config();

	Fields build_fields();
	map<string, Particles> build_particles();

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

	// Возвращает список диагностик для полей
	forward_list<diagnostic_up> diagnostics_list(multimap<string, vector<string>> description);
	// Возвращает список диагностик для частиц
	forward_list<diagnostic_up> diagnostics_list(string particle_name, vector<string> description);
};


//#################################################################################################

#endif
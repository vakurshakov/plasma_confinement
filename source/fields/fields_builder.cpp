#include "./fields_builder.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>

#include "fields.hpp"
#include "../solvers/FDTD.hpp"
#include "../constants.h"

// list of diagnostics
#include "../diagnostics/energy.hpp"
#include "../diagnostics/field_along_the_line.hpp"
#include "../diagnostics/field_at_point.hpp"
#include "../diagnostics/whole_field.hpp"


using std::vector, std::map, std::multimap, std::string, std::make_unique;
using v3f = vector3_field;
using v3f_up = std::unique_ptr<vector3_field>;
using diagnostic_up = std::unique_ptr<Diagnostic>;


std::function<void(v3f& E, v3f& B, v3f& j)> Fields_builder::propogator()
{
	std::cout << "\t\tSetting propogator...";
	// Вовзращает выбранный солвер (пропогатор) [ работает с файлом ./constants.h ]
	std::function<void(v3f& E, v3f& B, v3f& j)> propogator;
	
	if ( field_solver.empty() ) {
		std::cout << "what():  Initialization error: No field_solver in file [./constants.h]" << std::endl; 
 	}
	else if ( field_solver == "FDTD_2D" ) {
		propogator = FDTD_2D;
	}
	else {
		std::cout << "what():  Initialization error: No matching field_solver" << std::endl;
	}
	std::cout << "\tdone (" << field_solver << ")" << std::endl;
	return propogator;
}


v3f_up Fields_builder::load_field(string type)
{
	std::cout << "\t\tMaking a(an) " << type << " v3f...";
	// Возвращает сконфигурированные поля [ работает с файлом ./constants.h ]
	v3f_up field = nullptr;

	if ( boundaries.empty() ) {
		std::cout << "load_field:  Initialization error: No boundaries in file [./constants.h]" << std::endl;
	}
	else if ( boundaries == "px_py" ) {
		field = make_unique<px_py_vector3_field>(SIZE_X, SIZE_Y);
	}
	else if ( boundaries == "rx_ry" ) {
		field = make_unique<rx_ry_vector3_field>(type, SIZE_X, SIZE_Y);
	}
	else if ( boundaries == "rx_py" ) {
		field = make_unique<rx_py_vector3_field>(type, SIZE_X, SIZE_Y);
	}
	else if ( boundaries == "px_ry" ) {
		field = make_unique<px_ry_vector3_field>(type, SIZE_X, SIZE_Y);
	}
	else {
		std::cout << "load_field:  Initialization error: No matching boundaries" << std::endl;	
	}
	std::cout << "\tdone (" << field.get() << ")" << std::endl;
	return field;
}

Fields Fields_builder::build()
{
	#if there_are_fields
		std::cout << "\tBuilding fields:" << std::endl;

		Fields fields(	std::move(this->load_field("Electric")),
						std::move(this->load_field("Magnetic")),
						std::move(this->load_field("Electric")),
						std::move(this->propogator()),
						std::move(this->diagnostics_list(fields_diagnostics))	);
		std::cout << "\tBuilding of fields is complete\n" << std::endl;
		return fields;
	#else
		Fields fields();
		return fields;
	#endif
}

enum FD_DESC { FIELD, AXIS, PX, PY };


vector<diagnostic_up> Fields_builder::diagnostics_list(
	multimap<string, vector<string>> fields_diagnostics)
{
	std::cout << "\t\tSetting diagnostics...";
	// Возвращает список необходимых диагностик для полей
	vector<diagnostic_up> vec_diagnostics{};
	
	#if fields_are_diagnosed
		if ( fields_diagnostics.empty() ) {
			std::cout << "what():  Initialization error: fields are diagnosed "
					<< "but no fields_diagnostics in file [./constants.h]";
		}
		else {	
			for (auto& now : fields_diagnostics) {
				if ( now.first == "energy" ) {
					std::cout << "\n\t\t\t" << now.first;
					vec_diagnostics.emplace_back(make_unique<fields_energy>(dir_name + "/" + now.first));
				}
				else if ( now.first == "whole_field" ) {
					std::cout << "\n\t\t\t" << now.first;
					vec_diagnostics.emplace_back(make_unique<whole_field>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS] ) );
				}
				else if ( now.first == "field_along_x_axis" ) {
					std::cout << "\n\t\t\t" << now.first;
					vec_diagnostics.emplace_back(make_unique<field_along_x_axis>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS],
						stoi(now.second[FD_DESC::PX]) ) );
				}
				else if ( now.first == "field_along_y_axis" ) {
					std::cout << "\n\t\t\t" << now.first;
					vec_diagnostics.emplace_back(make_unique<field_along_y_axis>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS],
						stoi(now.second[FD_DESC::PX]) ) );
				} 
				else if ( now.first == "field_at_point" ) {
					std::cout << "\n\t\t\t" << now.first;
					vec_diagnostics.emplace_back(make_unique<field_at_point>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS], 
						stoi(now.second[FD_DESC::PX]), stoi(now.second[FD_DESC::PY]) ) );
				}
				else {
					std::cout << "what():  Initialization error: No matching diagnostics for fields." << std::endl;
				}
			}
		}
	#endif
	std::cout << "\n\t\tdone" << std::endl;
	
	vec_diagnostics.shrink_to_fit();
	return vec_diagnostics; 
}
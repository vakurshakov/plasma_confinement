#include "./fields_builder.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>

#include "./fields.hpp"
#include "../solvers/solvers.hpp"
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


function<void(v3f& E, v3f& B, v3f& j)> Fields_builder::propogator()
{
	// Вовзращает выбранный солвер (пропогатор) [ работает с файлом ./constants.h ]
	if ( field_solver.empty() ) {
		std::cout << "what():  Initialization error: No field_solver in file [./constants.h]" << std::endl; 
 	}
	else if ( field_solver == "FDTD_2D" ) {
		return FDTD_2D;			
	}
	else {
		std::cout << "what():  Initialization error: No matching field_solver" << std::endl;
	}
}


v3f_up Fields_builder::load_field(string type)
{
	// Возвращает сконфигурированные поля [ работает с файлом ./constants.h ]
	if ( boundaries.empty() ) {
		std::cout << "load_field:  Initialization error: No boundaries in file [./constants.h]" << std::endl;
	}
	else if ( boundaries == "px_py" ) {
		return make_unique<px_py_vector3_field>(SIZE_X, SIZE_Y);
	}
	else if ( boundaries == "rx_ry" ) {
		if ( type == "Electric" ) {
			return make_unique<rx_ry_Electric_field>(SIZE_X, SIZE_Y);
		}
		else if ( type == "Magnetic" ) {
			return make_unique<rx_ry_Magnetic_field>(SIZE_X, SIZE_Y);
		}
	}
	else if ( boundaries == "rx_py" ) {
		if ( type == "Electric" ) {
			return make_unique<rx_py_Electric_field>(SIZE_X, SIZE_Y);
		}
		else if ( type == "Magnetic" ) {
			return make_unique<rx_py_Magnetic_field>(SIZE_X, SIZE_Y);
		}
	}
	else if ( boundaries == "px_ry" ) {
		if ( type == "Electric" ) {
			return make_unique<px_ry_Electric_field>(SIZE_X, SIZE_Y);
		}
		else if ( type == "Magnetic" ) {
			return make_unique<px_ry_Magnetic_field>(SIZE_X, SIZE_Y);
		}
	}
	else {
		std::cout << "load_field:  Initialization error: No matching boundaries" << std::endl;	
	}
}

Fields Fields_builder::build()
{
	#if there_are_fields
		Fields fields(	this->load_field("Electric"),
						this->load_field("Magnetic"),
						this->load_field("Electric"),
						this->propogator(),
						this->diagnostics_list(fields_diagnostics)	);
		return fields;
	#else
		Fields fields();
		return fields;
	#endif
}

enum FD_DESC { FIELD, AXIS, PX, PY };


forward_list<diagnostic_up> Fields_builder::diagnostics_list(
	multimap<string, vector<string>> fields_diagnostics)
{
	// Возвращает список необходимых диагностик для полей
	forward_list<diagnostic_up> fl_diagnostics{};
	
	#if fields_are_diagnosed
		if ( fields_diagnostics.empty() ) {
			std::cout << "what():  Initialization error: fields are diagnosed "
					<< "but no fields_diagnostics in file [./constants.h]" << std::endl;
		}
		else {	
			for (auto& now : fields_diagnostics) {
				if ( now.first == "energy" ) {
					fl_diagnostics.push_front(make_unique<fields_energy>(dir_name + "/" + now.first));
				}
				else if ( now.first == "whole_field" ) {
					fl_diagnostics.push_front(make_unique<whole_field>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS] ) );
				}
				else if ( now.first == "field_along_x_axis" ) {
					fl_diagnostics.push_front(make_unique<field_along_x_axis>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS],
						stoi(now.second[FD_DESC::PX]) ) );
				}
				else if ( now.first == "field_along_y_axis" ) {
					fl_diagnostics.push_front(make_unique<field_along_y_axis>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS],
						stoi(now.second[FD_DESC::PX]) ) );
				} 
				else if ( now.first == "field_at_point" ) {
					fl_diagnostics.push_front(make_unique<field_at_point>(dir_name + "/fields/" + now.first,
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
	return fl_diagnostics; 
}
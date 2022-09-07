#include "./fields_builder.hpp"

#include "fields.hpp"
#include "../solvers/FDTD.hpp"
#include "../constants.h"

// list of diagnostics
#include "../diagnostics/energy.hpp"
#include "../diagnostics/field_along_the_line.hpp"
#include "../diagnostics/field_at_point.hpp"
#include "../diagnostics/whole_field.hpp"

using std::vector, std::map, std::multimap, std::string, std::make_unique;


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


std::unique_ptr<vector3_field> Fields_builder::load_field(string type)
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


vector<std::unique_ptr<Fields_diagnostic>>
Fields_builder::diagnostics_list(multimap<string, vector<string>> fields_diagnostics)
{
	// Возвращает список необходимых диагностик для полей
	std::cout << "\t\tSetting diagnostics...";

	vector<diagnostic_up> vec_diagnostics{};
	
	#if fields_are_diagnosed
		if ( fields_diagnostics.empty() ) {
			std::cout << "what():  Initialization error: fields are diagnosed "
					<< "but no fields_diagnostics in file [./constants.h]";
		}
		else {	
			enum FD_DESC { FIELD, AXIS, PX, PY };

			for (const auto& [name, description] : fields_diagnostics) {
				if ( name == "energy" ) {
					std::cout << "\n\t\t\t" << name;
					vec_diagnostics.emplace_back(make_unique<fields_energy>(dir_name + "/" + name));
				}
				else if ( name == "whole_field" ) {
					std::cout << "\n\t\t\t" << name;
					vec_diagnostics.emplace_back(make_unique<whole_field>(dir_name + "/fields/" + name,
						description[FD_DESC::FIELD] + description[FD_DESC::AXIS],
						description[FD_DESC::FIELD], description[FD_DESC::AXIS],
						1000, 2000, // int(SIZE_X/2 - 1.3*r_prop*r_larm/dx), int(SIZE_X/2 + 1.3*r_prop*r_larm/dx),
						1000, 2000)); // int(SIZE_Y/2 - 1.3*r_prop*r_larm/dy), int(SIZE_Y/2 + 1.3*r_prop*r_larm/dy)));
				}
				else if ( name == "field_along_x_axis" ) {
					std::cout << "\n\t\t\t" << name;
					vec_diagnostics.emplace_back(make_unique<field_along_x_axis>(dir_name + "/fields/" + name,
						description[FD_DESC::FIELD] + description[FD_DESC::AXIS],
						description[FD_DESC::FIELD], description[FD_DESC::AXIS],
						stoi(description[FD_DESC::PX]),
						int(SIZE_X/2 - 1.3*r_prop*r_larm/dx), int(SIZE_X/2 + 1.3*r_prop*r_larm/dx)));
				}
				else if ( name == "field_along_y_axis" ) {
					std::cout << "\n\t\t\t" << name;
					vec_diagnostics.emplace_back(make_unique<field_along_y_axis>(dir_name + "/fields/" + name,
						description[FD_DESC::FIELD] + description[FD_DESC::AXIS],
						description[FD_DESC::FIELD], description[FD_DESC::AXIS],
						stoi(description[FD_DESC::PX]),
						int(SIZE_Y/2 - 1.3*r_prop*r_larm/dy), int(SIZE_Y/2 + 1.3*r_prop*r_larm/dy) ));
				} 
				else if ( name == "field_at_point" ) {
					std::cout << "\n\t\t\t" << name;
					vec_diagnostics.emplace_back(make_unique<field_at_point>(dir_name + "/fields/" + name,
						description[FD_DESC::FIELD] + description[FD_DESC::AXIS],
						description[FD_DESC::FIELD], description[FD_DESC::AXIS], 
						stoi(description[FD_DESC::PX]), stoi(description[FD_DESC::PY])));
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
#ifndef FIELDS_BUILDER_HPP
#define FIELDS_BUILDER_HPP

//#################################################################################################

#include "fields.hpp"

#include "src/pch.h"
#include "../particles/particles.hpp"

using std::map, std::multimap, std::string, std::vector;
	

class Fields_builder {
public:
	Fields_builder() = default;
	Fields build();
	
private:
	// Методы для постороения полей
	using v3f = vector3_field;
	using v3f_up = std::unique_ptr<vector3_field>;
	std::function<void(v3f& E, v3f& B, v3f& j)> propogator();
	v3f_up load_field(string type);

	// Возвращает список диагностик
	using diagnostic_up = std::unique_ptr<Fields_diagnostic>;
	vector<diagnostic_up> diagnostics_list(multimap<string, vector<string>> description);
};


//#################################################################################################

#endif //FIELDS_BUILDER_HPP
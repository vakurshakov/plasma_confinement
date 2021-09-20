#ifndef DIAGNOSTICS_DIAGNOSTIC_HPP
#define DIAGNOSTICS_DIAGNOSTIC_HPP

//#################################################################################################

#include <string>
#include <vector>
#include <memory>

#include "../file_writers/file_interface.hpp"
#include "../file_writers/bin_file.hpp"
#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../vectors/vector3_field.hpp"

using v3f = vector3_field;


/*
	NOTE: Скорее всего нужно будет ещё пересмотреть систему диагностирования, чтобы 
		она не брала на вход слишком много. Так, не стоит передавать в диагностику сразу
		все поля; это убирает возможность создать объект диагностики для конкретного
		(одного) поля, а также добавляет неясность в том, для какого поля эта диагностика
		на самом деле.

		Чтобы этого избежать нужно разделить сильнее рабочие сущности и диагностики.
*/

class Diagnostic {
public:
	Diagnostic() = default;

	Diagnostic(std::string directory_path, std::string file_name) 
		: file_for_results_(new BIN_File(directory_path, file_name)) {};

	virtual ~Diagnostic() = default;

	virtual void save_parameters(std::string directory_path) = 0;
	virtual void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) {};
	virtual void diagnose(const Particle_parameters&, const std::vector<Point>&, int t) {};

protected:

	std::unique_ptr<IFile> file_for_results_ = nullptr;
};


//#################################################################################################

#endif //DIAGNOSTICS_DIAGNOSTIC_HPP

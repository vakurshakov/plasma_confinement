#ifndef DIAGNOSTICS_DIAGNOSTIC_HPP
#define DIAGNOSTICS_DIAGNOSTIC_HPP

//#################################################################################################

#include <string>
#include <vector>
#include <memory>

#include "../file_writers/file_interface.hpp"
#include "../file_writers/bin_file.hpp"
#include "../particles/particle/particle.hpp"
#include "../vectors/vector3_field.hpp"


/*
	TODO: Нужно будет ещё пересмотреть систему диагностирования, чтобы она не 
		брала на вход слишком много. Так, не стоит передавать в диагностику сразу
		все поля; это убирает возможность создать объект диагностики для конкретного
		(одного) поля, а также добавляет неясность в том, для какого поля эта диагностика
		на самом деле.

		Чтобы этого избежать нужно разделить сильнее рабочие сущности и диагностики.
*/

class Diagnostic {
public:
	Diagnostic() = default;

	Diagnostic(std::string directory_path) 
		: directory_path_(directory_path) {};

	virtual ~Diagnostic() = default;

	virtual void save_parameters(std::string directory_path) = 0;

protected:
	std::string directory_path_;
	std::unique_ptr<IFile> file_for_results_ = nullptr;
};


class Particles_diagnostic : public Diagnostic {
public:
	Particles_diagnostic(std::string directory_path)
		: Diagnostic(directory_path) {};

	virtual void diagnose(const Parameters&, const std::vector<Particle>&, int t) = 0;
};


class Fields_diagnostic : public Diagnostic {
public:
	Fields_diagnostic(std::string directory_path)
		: Diagnostic(directory_path) {};

	using v3f = vector3_field;
	virtual void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) = 0;
};

//#################################################################################################

#endif //DIAGNOSTICS_DIAGNOSTIC_HPP

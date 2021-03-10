#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

//#################################################################################################

#include "../vectors/vector3_field.hpp"
#include "../particles/species_description.hpp"
#include "../constants.h"

#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <cmath>
#include <omp.h>
#include <dir.h>


using namespace std;
using v3f = vector3_field;

class Diagnostic {
public:
	Diagnostic() = default;

	Diagnostic(string path, string name_)
		: path_(path), name_(name_) {
		
		mkdir(path_.c_str());
		ofs_.open((path_ + "/" + name_ + ".txt").c_str());

		ofs_ << TIME << " " << dt << endl;
		ofs_ << setprecision(10) << fixed;
	}
                      
	virtual ~Diagnostic() = default;
	virtual void initialize() = 0;
	virtual void diagnose(const v3f& E, const v3f& B, const v3f& j) {};
	virtual void diagnose(const Species_description& sort) {};

protected:
	string path_;
	string name_;
	ofstream ofs_;
};


class fields_energy : public Diagnostic {
public:
	fields_energy(string path) : Diagnostic(path, "fields_energy") {
		initialize();
	}

	void initialize() override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j) override;
	
private:
	double W = 0;
};

class particles_energy : public Diagnostic {
public:
	particles_energy(string path) : Diagnostic(path, "particles_energy") {
		initialize();
	};

	void initialize() override;
	void diagnose(const Species_description& sort) override;
	
private:
	double W = 0;
};

class whole_field : public Diagnostic {
public:
	whole_field(string path, string name, string field, string axis)
	: Diagnostic(path, name), field_(field), axis_(axis) {
		initialize();
	};

	void initialize() override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j) override;
	
private:
	string field_;
	string axis_;
};

class field_along_X : public Diagnostic {
public:
	field_along_X(string path, string name, string field, string axis, int Y)
	: Diagnostic(path, name), field_(field), axis_(axis), Y_(Y) {
		initialize();
	};

	void initialize() override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j) override;
	
private:
	string field_;
	string axis_;
	int Y_;
};

class field_along_Y : public Diagnostic {
public:
	field_along_Y(string path, string name, string field, string axis, int X)
	: Diagnostic(path, name), field_(field), axis_(axis), X_(X) {
		initialize();
	};

	void initialize() override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j) override;
	
private:
	string field_;
	string axis_;
	int X_;
};

class field_at_point : public Diagnostic {
public:
	field_at_point(string path, string name, string field, string axis, int px, int py)
	: Diagnostic(path, name), field_(field), axis_(axis), px_(px), py_(py) {
		initialize();
	};

	void initialize() override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j) override;
	
private:
	string field_;
	string axis_;
	int px_;
	int py_;
};

class phase_diagram : public Diagnostic {
public:
	phase_diagram(string path) : Diagnostic(path, "phase_diagram") {
		initialize();
	};

	void initialize() override;
	void diagnose(const Species_description& sort) override;
	
private:
	double vmin_ = 0;
	double vmax_ = 1;
	double dv_ = 1./20.;
	
	int xmin_ = 0;
	int xmax_ = SIZE_X;		
};

//#################################################################################################

#endif

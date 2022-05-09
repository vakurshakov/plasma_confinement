#ifndef MAGNETIC_FIELD_HALF_STEP
#define MAGNETIC_FIELD_HALF_STEP

//#######################################################################################

#include "command.hpp"

#include "../fields/fields.hpp"


class Magnetic_field_half_step : public Command {
public:
	Magnetic_field_half_step(Fields* fields)
		: fields_(fields) {};
		
	void execute(int t) const override;

private:
	Fields* const fields_;	
	double Bz0_;
};

//#######################################################################################

#endif // MAGNETIC_FIELD_HALF_STEPa
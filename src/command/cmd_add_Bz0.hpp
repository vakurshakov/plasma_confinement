#ifndef CMD_ADD_BZ0_HPP
#define CMD_ADD_BZ0_HPP

//#######################################################################################


#include "./command.hpp"

#include "../vectors/vector3_field.hpp"

using v3f = vector3_field;


class Add_Bz0 : public Command {
public:
	explicit Add_Bz0(Fields* fields, double Bz0)
		: fields_(fields), Bz0_(Bz0);
		
	void execute() const override {
		(fields_->B()).add_Bz0(Bz0);
	};

private:
	Fields *fields_;	
	double Bz0_;
};

//#######################################################################################

#endif // CMD_ADD_BZ0_HPP
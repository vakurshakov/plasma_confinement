#include "./command.hpp"

#include "../vectors/vector3_field.hpp"

using v3f = vector3_field;


class Add_Bz0 : public Command {
public:
	explicit Add_Bz0(Fields*, double Bz0);

	void execute() override {
		Fields->add_Bz0(Bz0);
	};

private:
	Fields *fields_;	
	double Bz0;
};
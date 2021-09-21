#ifndef COMMAND_COMMAND_HPP
#define COMMAND_COMMAND_HPP

//#######################################################################################


#include "../particle/point.hpp"
#include "../vectors/vector_classes.hpp"


class Fields_command {
public:
	virtual ~Fields_command() = default;
	virtual void execute() const = 0;
};

class Particle_command {
public: 
	virtual ~Particle_command() = default;
	virtual void execute(Point*, const vector2& r0) const = 0;
};


//#######################################################################################

#endif // COMMAND_COMMAND_HPP
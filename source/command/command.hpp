#ifndef COMMAND_COMMAND_HPP
#define COMMAND_COMMAND_HPP

//#######################################################################################

#include "../particles/particle/point.hpp"
#include "../vectors/vector_classes.hpp"


class Command {
public:
	virtual ~Command() = default;
	virtual void execute(int t) const = 0;
};


//#######################################################################################

#endif // COMMAND_COMMAND_HPP 
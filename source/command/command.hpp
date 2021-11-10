#ifndef COMMAND_COMMAND_HPP
#define COMMAND_COMMAND_HPP

//#######################################################################################

#include "../particles/particle/point.hpp"
#include "../vectors/vector_classes.hpp"

/**
 * @brief We will use command pattern in two situation:  
 *		1. To do presets before main calculation cycle;
 *		2. To do presets on each step of the calculation. 
 *
 * @param t Time from the main cycle.
 */
class Command {
public:
	virtual ~Command() = default;
	virtual void execute(int t) const = 0;
	bool this_is_time_to_remove(int t) const { return false; }
};


//#######################################################################################

#endif // COMMAND_COMMAND_HPP 
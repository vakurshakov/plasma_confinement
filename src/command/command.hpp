#ifndef COMMAND_COMMAND_HPP
#define COMMAND_COMMAND_HPP

//#######################################################################################

#include "../particles/particle/point.hpp"
#include "../vectors/vector_classes.hpp"

/**
 * Interface of an abstract command
 * 
 * @brief We will use command pattern in two situation:  
 *		1. To do presets before main calculation cycle;
 *		2. To do presets on each step of the calculation. 
 *
 * @param t Time from the main cycle.
 */
class Command {
public:
	virtual ~Command() = default;

	/**
	 * Executes command.
	 * 
	 * @param t Outer time step, optional.
	 */
	virtual void execute(int t) const = 0;

	/**
	 * @brief Checks whether command needs to be removed
	 * 		from the command list or not. 
	 * 
	 * @param t Outer time step, base of desicion.
	 * 
	 * @return True or false with a reference to time t
	 * 		and based on the implementation.
	 */
	virtual bool needs_to_be_removed(int t) const { return false; }
};


//#######################################################################################

#endif // COMMAND_COMMAND_HPP 
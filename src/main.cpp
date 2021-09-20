#include "./managers/configuration_saver.hpp"
#include "./managers/manager.hpp"


int main(int argc, char const *argv[])
{
	{
		Configuration_saver configuration_saver;
		configuration_saver.save();
	}

	Manager Nerpa;
	Nerpa.initializes();
	Nerpa.calculates();

	return 0;
}
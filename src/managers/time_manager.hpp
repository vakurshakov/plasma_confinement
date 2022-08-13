#include <chrono>
#include <iostream>
#include <string>

#include "../constants.h"


namespace ch = std::chrono;


class Timer {
public:
	Timer() = default;

	void set_up() {
		start_ = ch::steady_clock::now();
		state = "set up";
	}
	
	void tick(int t) {
		if (!(state == "set up")) {
			std::cout << "Timer::tick(int): Timer wasn't set up" << std::endl;
		}
		else {
		if ( t % (TIME/10) == 0 ) {
			if ( t == 0 ) {
				std::cout << std::endl;
			}
			
			auto current_time = ch::steady_clock::now();
			ch::duration<double> elapsed_time = current_time - start_;
			
			std::cout << "\t[";
			for (int n = 0; n <= TIME; n+= TIME/10) {
				if ( n <= t ) { std::cout << "#"; }
				else { std::cout << " "; }
			}
			std::cout << "]\t" << elapsed_time.count() << "s" << std::endl;
		}
		}
	}

	void elapsed() {
		if (!(state == "set up")) {
			std::cout << "Timer::tick(int): Timer wasn't set up" << std::endl;
		}
		else {
			auto end = ch::steady_clock::now();
			ch::duration<double> elapsed_time = end - start_;
			std::cout << "\n\n\truntime:\t" << elapsed_time.count() << "s\n" << std::endl;
		}
	}

private:
	decltype(ch::steady_clock::now()) start_;
	std::string state = "set down";
};
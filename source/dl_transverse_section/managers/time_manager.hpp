#include <chrono>
#include <iostream>

#include "../constants.h"

using std::chrono;


class Timer {
public:
	Timer() = default;

	set_up() { start_ = steady_clock::now(); }
	tick(int t) {
		if ( t % (TIME/10) == 0 ) {
			if ( t == 0 ) std::cout << std::endl;
		
			auto current_time = steady_clock::now();
			if ( start == 0 ) {
				std::cout << "Timer::tick(int): Timer wasn't set up" << std::cout;
			}
			else{
				auto elapsed = duration_cast<minutes>(current_time - start);
			
				std::cout << "\t[";
					for (int n = 0; n <= TIME; n+= TIME/10) {
						if ( n <= t ) { cout << "#"; }
						else { cout << " "; }
					}
				std::cout << "]\t" << elapsed.count() << "min" << std::endl;
			}
		}
	}

private:
	decltype(steady_clock::now()) start_;
};
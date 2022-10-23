#ifndef CONFIGURATION_SAVER_HPP
#define CONFIGURATION_SAVER_HPP

#include "src/pch.h"

#include <iostream>

namespace fs = std::filesystem;

class Configuration_saver {
public:
	Configuration_saver() = default;
	void save() {
		// Сохранение параметров (/*from*/ constants.h, /*to*/ dir_name)
		try{
			fs::copy("./src/constants.h", dir_name + "/constants.h");
		}
		catch(fs::filesystem_error const& ex) {
			switch(ex.code().value()) {
				case 2: // "No such file or directory"
					fs::create_directories(dir_name);
					fs::copy("./src/constants.h", dir_name + "/constants.h");
					break;
				case 17: // "File exists"
					fs::remove(dir_name + "/constants.h");
					fs::copy("./src/constants.h", dir_name + "/constants.h");
					break;
				default:
					std::cout
						<< "what():  " << ex.what() << '\n'
						<< "path1(): " << ex.path1() << '\n'
						<< "path2(): " << ex.path2() << '\n'
						<< "code().value():    " << ex.code().value() << '\n'
						<< "code().message():  " << ex.code().message() << '\n'
						<< "code().category(): " << ex.code().category().name() << std::endl;
					break;
			}
		}
	}
};


#endif // CONFIGURATION_SAVER_HPP
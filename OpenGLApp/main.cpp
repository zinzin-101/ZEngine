#include "Engine.h"

#include <exception>
#include <iostream>
#include <stdexcept>

int main() {
	try {
		Engine engine;
		engine.run();
	}
	catch (const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
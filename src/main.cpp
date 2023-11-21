#include "app/application.hpp"
#include "ft_glm/ft_glm.hpp"

#include "logger.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char **argv) {

	ft::Application app;

	try {
		app.run();
	} catch (const std::exception &e) {
		ft::log << ft::Logger::Level::ERROR << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
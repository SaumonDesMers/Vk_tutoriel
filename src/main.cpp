#include "application.hpp"
#include "ft_glm/ft_glm.hpp"

#include "logger.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char **argv) {

	ft::log.setTimestamp(false);

	try {
		ft::Application app;

		app.run();
	} catch (const std::exception &e) {
		ft::log << ft::Logger::Level::ERROR << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
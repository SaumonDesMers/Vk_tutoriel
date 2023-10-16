#include "application.hpp"
#include "ft_glm/ft_glm.hpp"

#include "../tests/ft_glm_test.hpp"
#include <glm/glm.hpp>

int main(int argc, char **argv) {

	// test_ft_glm();
	// return EXIT_SUCCESS;

	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <model_path>" << " <texture_path>" << std::endl;
		return EXIT_FAILURE;
	}

	Application app;

	app.setModelPath(argv[1]);
	app.setTexturePath(argv[2]);

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
#include "app/application.hpp"
#include "ft_glm/ft_glm.hpp"


int main(int argc, char **argv) {


	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <model_path>" << " <texture_path>" << std::endl;
		return EXIT_FAILURE;
	}

	Application app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
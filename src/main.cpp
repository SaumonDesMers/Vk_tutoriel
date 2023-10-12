#include "application.hpp"
#include "ft_glm/ft_glm.hpp"

#include "../tests/ft_glm_test.hpp"
#include <glm/glm.hpp>

int main() {
	srand(time(NULL));

	// test_ft_glm();
	// return EXIT_SUCCESS;

	ft::mat4 translate = ft::translate(ft::vec3(1.0f, 2.0f, 3.0f));
	glm::mat4 glm_translate = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));

	ft::log_mat4(translate);
	ft::log_mat4(glm_translate);

	Application app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
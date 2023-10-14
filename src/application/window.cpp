#include "application.hpp"

void Application::initWindow() {
	glfwInit();

	/* Do not create openGL context */
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	this->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	/* Set pointer to this class as user pointer */
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, this->framebufferResizeCallback);

	/* Set key callback */
	this->mapKeyCallback();
	glfwSetKeyCallback(window, &Application::keyCallback);
}

void Application::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void Application::createSurface() {
	if (glfwCreateWindowSurface(this->instance, this->window, nullptr, &this->surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

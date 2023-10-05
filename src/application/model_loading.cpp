#include "application.hpp"
#include "model_loading.hpp"

// #define TINYOBJLOADER_IMPLEMENTATION
// #include <tiny_obj_loader.h>

#include <unordered_map>

void Application::loadModel() {

	ModelLoading modelLoading;

	modelLoading.loadModel(MODEL_PATH);
	modelLoading.populateVerticesAndIndices(this->vertices, this->indices);

}
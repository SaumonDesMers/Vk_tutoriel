#include "application.hpp"
#include "obj_loader.hpp"

// #define TINYOBJLOADER_IMPLEMENTATION
// #include <tiny_obj_loader.h>

#include <unordered_map>

void Application::loadModel() {

	ObjLoader modelLoading;

	modelLoading.loadModel(MODEL_PATH);
	modelLoading.populateVerticesAndIndices(this->vertices, this->indices);

}
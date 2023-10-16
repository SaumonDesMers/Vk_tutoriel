#include "application.hpp"
#include "obj_loader.hpp"

#include <unordered_map>

void Application::loadModel() {

	ObjLoader modelLoading;

	modelLoading.loadModel(this->model_path);
	this->object = modelLoading.createObject();

}
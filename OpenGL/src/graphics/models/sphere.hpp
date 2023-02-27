#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../model.h"
#include <iostream>

#include "modelarray.hpp"

class Sphere : public Model {
public:
	Sphere(glm::vec3 pos = glm::vec3(0.f), glm::vec3 size = glm::vec3(1.0f))
		: Model(BoundTypes::SPHERE, pos, size, true) {
		rb.mass = 1.0f;
		std::cout << Model::size.x << std::endl;
	}

	void init() {
		loadModel("assets/models/sphere/scene.gltf");
	}
};

class SphereArray : public ModelArray<Sphere> {
public:
	void init() {
		model = Sphere(glm::vec3(0.0f), glm::vec3(0.25f));
		ModelArray::init();
	}
};

#endif
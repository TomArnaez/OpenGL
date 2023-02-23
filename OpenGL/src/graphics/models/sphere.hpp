#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../model.h"

class Sphere : public Model {
public:
	Sphere(glm::vec3 pos = glm::vec3(0.f), glm::vec3 size = glm::vec3(1.0f))
		: Model(pos, size, true) {}

	void init() {
		loadModel("assets/models/sphere/scene.gltf");
	}
};

#endif
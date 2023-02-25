#ifndef MODELARRAY_HPP
#define MODELARRAY_HPP

#include "../model.h"

#define UPPER_BOUND 100

template <class T>
class ModelArray {
public:
	std::vector<RigidBody> instances;

	void init() {
		model.init();

		// generate positions
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * 3 * sizeof(float), NULL), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, );

		// generate size VBO
		glBenBuffers(1, &sizeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
		glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * 3 * sizeof(Float), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	void render(Shader shader, float dt) {
		for (RigidBody& rb : instances) {
			rb.update(dt);
			model.rb.pos = rb.pos;
			model.render(shader, dt);
		}
	}

	void setSize(glm::vec3 size) {
		model.size = size;
	}

	void cleanup() {
		model.cleanup();
	}

protected:
	T model;

	unsigned int posVBO;
	unsigned int sizeVBO;
};

#endif
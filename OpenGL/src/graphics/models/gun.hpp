#include "../model.h"
#include "../../io/camera.h"
#include "../../io/keyboard.h"

class Gun : public Model {
public:
	Gun()
		: Model(BoundTypes::AABB, glm::vec3(0.0f), glm::vec3(1 / 300.0f), true) {}

	void render(Shader shader, float dt, Box *box, bool setModel = false) {
		glm::mat4 model = glm::mat4(1.0f);

		// set position
		rb.pos = Camera::defaultCamera.cameraPos + glm::vec3(Camera::defaultCamera.cameraFront * 0.5f) - glm::vec3(Camera::defaultCamera.cameraUp * 0.205f);
		model = glm::translate(model, rb.pos);

		float theta;
		// rotate around camera right using dot product
		theta = acos(glm::dot(Camera::defaultCamera.worldUp, Camera::defaultCamera.cameraFront) /
			glm::length(Camera::defaultCamera.cameraUp) / glm::length(Camera::defaultCamera.cameraFront));
		model = glm::rotate(model, atanf(1) * 2 - theta, Camera::defaultCamera.cameraRight);// offset by pi / 2 bc angle angle btwn camFront and gunFront

		// rotate around cameraUp using dot product
		glm::vec2 front2d = glm::vec2(Camera::defaultCamera.cameraFront.x, Camera::defaultCamera.cameraFront.z);

		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), front2d) / glm::length(front2d));
		model = glm::rotate(model, Camera::defaultCamera.cameraFront.z < 0 ? theta : -theta, Camera::defaultCamera.worldUp);

		if (Keyboard::key(GLFW_KEY_SPACE)) {
			std::cout << Camera::defaultCamera.cameraFront.x << ' ' <<
				Camera::defaultCamera.cameraFront.y << ' ' <<
				Camera::defaultCamera.cameraFront.z << ' ' <<
				Camera::defaultCamera.cameraUp.x << ' ' <<
				Camera::defaultCamera.cameraUp.y << ' ' <<
				Camera::defaultCamera.cameraUp.z << ' ' << std::endl;
		}

		// scale
		model = glm::scale(model, size);

		shader.setMat4("model", model);

		Model::render(shader, dt, box, false);
	}
};
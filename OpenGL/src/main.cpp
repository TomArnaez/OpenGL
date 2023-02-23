#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>  

#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"


#include "graphics/model.h"
#include "graphics/light.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/camera.h"
#include "io/screen.h"


void process_input(double dt);

float mixVal = 0.5f;

glm::mat4 transform = glm::mat4(1.0f);

Screen screen;

// setup default
Camera Camera::defaultCamera(glm::vec3(0.0f, 0.0f, 0.0f));

Camera cameras[2] = {
    Camera(glm::vec3(0.0f, 0.0f, 0.0f)),
    Camera(glm::vec3(10.0f, 10.0f, 10.0f))
};

int activeCam = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool flashLightOn = true;

int main() {
	int success;
	char infoLog[512];

	std::cout << "Hello, OpenGL!" << std::endl;

	glfwInit();

	// openGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

# ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COPMPAT, GL_TRUE);
#endif

	if (!screen.init()) {
		std::cout << "Could not open window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	screen.setParameters();

	// SHADERS===============================
	Shader shader("assets/object.vs", "assets/object.fs");
	Shader lampShader("assets/object.vs", "assets/lamp.fs");

	// MODELS==============================
	//Gun g;
	//g.loadModel("assets/models/m4a1/scene.gltf");

	Sphere sphere(glm::vec3(0.0f), glm::vec3(0.25f));
	sphere.init();

	// LIGHTS
	DirLight dirLight = { glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f) };

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	Lamp lamps[4];
	for (unsigned int i = 0; i < 4; i++) {
		lamps[i] = Lamp(glm::vec3(1.0f),
			glm::vec4(0.05f, 0.05f, 0.05f, 1.0f), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), glm::vec4(1.0f),
			1.0f, 0.07f, 0.032f,
			pointLightPositions[i], glm::vec3(0.25f));
		lamps[i].init();
	}

	SpotLight s = {
		Camera::defaultCamera.cameraPos, Camera::defaultCamera.cameraFront,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)
	};


	while (!screen.shouldClose()) {
		// calculate dt
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		// process input
		process_input(deltaTime);

		// render
		screen.update();

		// draw shapes
		shader.activate();

		shader.set3Float("viewPos", Camera::defaultCamera.cameraPos);

		dirLight.render(shader);

		for (unsigned int i = 0; i < 4; i++) {
			lamps[i].pointLight.render(shader, i);
		}
		shader.setInt("noPointLights", 4);

		if (flashLightOn) {
			s.position = Camera::defaultCamera.cameraPos;
			s.direction = Camera::defaultCamera.cameraFront;
			s.render(shader, 0);
			shader.setInt("noSpotLights", 1);
		}
		else {
			shader.setInt("noSpotLights", 0);
		}

		// create transformation
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = Camera::defaultCamera.getViewMatrix();
		projection = glm::perspective(
			glm::radians(Camera::defaultCamera.zoom),
			(float)Screen::SCR_WIDTH / (float)Screen::SCR_HEIGHT, 0.1f, 100.0f);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		sphere.render(shader, deltaTime);

		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);

		for (unsigned int i = 0; i < 4; i++) {
			lamps[i].render(lampShader, deltaTime);
		}

		// send new frame to window
		screen.newFrame();
	}

	sphere.cleanup();

	for (unsigned int i = 0; i < 4; i++) {
		lamps[i].cleanup();
	}

	glfwTerminate();
	return 0;
}


void process_input(double dt) {
    if (Keyboard::key(GLFW_KEY_ESCAPE)) {
        screen.setShouldClose(true);
    }

    if (Keyboard::keyWentDown(GLFW_KEY_L)) {
        flashLightOn = !flashLightOn;
    }

    // change mix value
    if (Keyboard::keyWentDown(GLFW_KEY_UP)) {
        mixVal += .05f;
        if (mixVal > 1) {
            mixVal = 1.0f;
        }
    }

    if (Keyboard::keyWentDown(GLFW_KEY_DOWN)) {
        mixVal -= .05f;
        if (mixVal < 0) {
            mixVal = 0.0f;
        }
    }

    if (Keyboard::keyWentDown(GLFW_KEY_TAB)) {
        activeCam += (activeCam == 0) ? 1 : -1;
    }

    // move camera
    if (Keyboard::key(GLFW_KEY_W)) {
        Camera::defaultCamera.updateCameraPos(CameraDirection::FORWARD, dt);
    }
    if (Keyboard::key(GLFW_KEY_A)) {
        Camera::defaultCamera.updateCameraPos(CameraDirection::LEFT, dt);
    }
    if (Keyboard::key(GLFW_KEY_S)) {
        Camera::defaultCamera.updateCameraPos(CameraDirection::BACKWARD, dt);
    }
    if (Keyboard::key(GLFW_KEY_D)) {
        Camera::defaultCamera.updateCameraPos(CameraDirection::RIGHT, dt);
    }
    if (Keyboard::key(GLFW_KEY_SPACE)) {
        Camera::defaultCamera.updateCameraPos(CameraDirection::UP, dt);
    }
    if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
        Camera::defaultCamera.updateCameraPos(CameraDirection::DOWN, dt);
    }

    double dx = Mouse::getDX(), dy = Mouse::getDY();
    if (dx != 0 || dy != 0) {
        Camera::defaultCamera.updateCameraDirection(dx, dy);
    }

    double scrollDy = Mouse::getScrollDY();
    if (scrollDy != 0) {
        Camera::defaultCamera.updateCameraZoom(scrollDy);
    }
}
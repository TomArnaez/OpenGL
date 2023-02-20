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
#include "graphics/shader.h"
#include "graphics/texture.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/camera.h"
#include "io/screen.h"

void process_input(double dt);

float mixVal = 0.5f;

glm::mat4 transform = glm::mat4(1.0f);

unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;

Screen screen;

Camera cameras[2] = {
    Camera(glm::vec3(0.0f, 0.0f, 3.0f)),
    Camera(glm::vec3(10.0f, 10.0f, 10.0f))
};

int activeCam = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    glfwInit();

    // openGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

# ifdef __APPLE__
    glfwWidnowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (!screen.init()) {
        std::cout << "Could not create window." << std::endl;
        glfwTerminate();
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialise GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    screen.setParameters();

    glEnable(GL_DEPTH_TEST);

    // shaders

    Shader shader("assets/object.vs", "assets/object.fs");
    Shader lampShader("assets/object.vs", "assets/lamp.fs");

    Cube cube(Material::emerald, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.75f));
    cube.init();

    Lamp lamp(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(-1.0f, -0.5f, -0.5f), glm::vec3(0.25f));
    lamp.init();

    while (!screen.shouldClose()) {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        process_input(deltaTime);

        screen.update();

        shader.activate();
        shader.set3Float("light.position", lamp.pos);
        shader.set3Float("viewPos", cameras[activeCam].cameraPos);

        shader.set3Float("light.ambient", lamp.ambient);
        shader.set3Float("light.diffuse", lamp.diffuse);
        shader.set3Float("light.specular", lamp.specular);

        // create transformation to screen
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = cameras[activeCam].getViewMatrix();
        projection = glm::perspective(glm::radians(cameras[activeCam].getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        cube.render(shader);

        lampShader.activate();
        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);
        lamp.render(lampShader);

        screen.newFrame();
    }

    glfwTerminate();
    return 0;
}

void process_input(double dt) {
    if (Keyboard::key(GLFW_KEY_ESCAPE)) {
        screen.setShouldClose(true);
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

    if (Keyboard::key(GLFW_KEY_W)) {
        transform = glm::translate(transform, glm::vec3(0.0f, 0.1f, 0.0f));
    }
    if (Keyboard::key(GLFW_KEY_A)) {
        transform = glm::translate(transform, glm::vec3(-0.1f, 0.0f, 0.0f));
    }
    if (Keyboard::key(GLFW_KEY_S)) {
        transform = glm::translate(transform, glm::vec3(0.0f, -0.1f, 0.0f));
    }
    if (Keyboard::key(GLFW_KEY_D)) {
        transform = glm::translate(transform, glm::vec3(0.1f, 0.0f, 0.0f));
    }

    // move camera
    if (Keyboard::key(GLFW_KEY_W)) {
        cameras[activeCam].updateCameraPos(CameraDirection::FORWARD, dt);
    }
    if (Keyboard::key(GLFW_KEY_A)) {
        cameras[activeCam].updateCameraPos(CameraDirection::LEFT, dt);
    }
    if (Keyboard::key(GLFW_KEY_S)) {
        cameras[activeCam].updateCameraPos(CameraDirection::BACKWARD, dt);
    }
    if (Keyboard::key(GLFW_KEY_D)) {
        cameras[activeCam].updateCameraPos(CameraDirection::RIGHT, dt);
    }
    if (Keyboard::key(GLFW_KEY_SPACE)) {
        cameras[activeCam].updateCameraPos(CameraDirection::UP, dt);
    }
    if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
        cameras[activeCam].updateCameraPos(CameraDirection::DOWN, dt);
    }

    double dx = Mouse::getDX(), dy = Mouse::getDY();
    if (dx != 0 || dy != 0) {
        cameras[activeCam].updateCameraDirection(dx, dy);
    }

    double scrollDy = Mouse::getScrollDY();
    if (scrollDy != 0) {
        cameras[activeCam].updateCameraZoom(scrollDy);
    }
}
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

unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;

Screen screen;

Camera cameras[2] = {
    Camera(glm::vec3(0.0f, 0.0f, 3.0f)),
    Camera(glm::vec3(10.0f, 10.0f, 10.0f))
};

int activeCam = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool flashLightOn = true;

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

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    Cube cubes[10];
    for (unsigned int i = 0; i < 10; i++) {
        cubes[i] = Cube(Material::gold, cubePositions[i], glm::vec3(1.0f));
        cubes[i].init();
    }

    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f,  0.2f,  2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3(0.0f,  0.0f, -3.0f)
    };

    Lamp lamps[4];
    for (unsigned int i = 0; i < 4; i++) {
        lamps[i] = Lamp(glm::vec3(1.0f),
            glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f),
            1.0f, 0.07f, 0.032f,
            pointLightPositions[i], glm::vec3(0.25f));
        lamps[i].init();
    }

    //DirLight dirLight = { glm::vec3(-0.2f, -10.f, -0.3f), glm::vec3(0.1f), glm::vec3(0.4f), glm::vec3(0.45) };
    SpotLight spotLight = { cameras[activeCam].cameraPos, cameras[activeCam].cameraFront,
    glm::cos(glm::radians(125.f)), glm::cos(glm::radians(20.0f)),
        1.0f, 0.07f, 0.03f,
        glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f) };


    while (!screen.shouldClose()) {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        process_input(deltaTime);

        screen.update();

        shader.activate();
        shader.set3Float("viewPos", cameras[activeCam].cameraPos);

        // shader.set3Float("light.position", lamp.pos);
        //dirLight.render(shader);

        for (int i = 0; i < 4; i++) {
            lamps[i].pointLight.render(shader, i);
        }
        shader.setInt("noPointLights", 4);

        if (flashLightOn) {
            spotLight.position = cameras[activeCam].cameraPos;
            spotLight.direction = cameras[activeCam].cameraFront;
            spotLight.render(shader, 0);
            shader.setInt("noSpotLights", 1);
        }
        else {
            shader.setInt("noSpotLights", 0);
        }

        // create transformation to screen
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = cameras[activeCam].getViewMatrix();
        projection = glm::perspective(glm::radians(cameras[activeCam].getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        for (int i = 0; i < 10; ++i) {
            cubes[i].render(shader);
        }

        lampShader.activate();
        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);

        for (int i = 0; i < 4; ++i) {
            lamps[i].render(lampShader);
        }
        
        screen.newFrame();
    }
    for (int i = 0; i < 10; ++i) {
        cubes[i].cleanup();
    }

    for (int i = 0; i < 10; ++i) {
        lamps[i].cleanup();
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
#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "shader.h"

struct PointLight {
	glm::vec3 position;

	// attenuation constants
	float k0;
	float k1;
	float k2;
	
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};

struct DirLight {
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader);
};

struct SpotLight {
	glm::vec3 direction;
	glm::vec3 position;

	// attenuation constants
	float k0;
	float k1;
	float k2;

	float cutOff;
	float outerCutOff;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};

#endif
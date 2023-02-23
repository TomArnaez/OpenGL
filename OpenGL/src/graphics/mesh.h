#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/scene.h>
#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	static std::vector<Vertex> genList(float* vertices, int noVertices);
};
typedef struct Vertex Vertex;

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO;

	std::vector<Texture> textures;
	aiColor4D diffuse;
	aiColor4D specular;

	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, aiColor4D diffuse, aiColor4D specular);

	void render(Shader shader);

	void cleanup();

private:
	unsigned int VBO, EBO;

	bool noTex;

	void setup();
};
#endif
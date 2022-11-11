#ifndef GAMEOBJECTVERTICES_H
#define GAMEOBJECTVERTICES_H

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "tools.h"

class GameObjectVertices
{
public:
	GameObjectVertices(char* objFileName);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> normals;

	GLuint bufVertices;
	GLuint bufNormals;
	GLuint bufTexCoords;
	GLuint bufIndeces;

	GLuint vao;
	std::vector<unsigned short> indices;
};

#endif
#ifndef TOOLS_H
#define TOOLS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.h"
#include "lodepng.h"
#include "objloader.h"

GLuint readTexture(char* filename);
GLuint makeBuffer(void *data, int vertexCount, int vertexsize);
void assignVBOtoAttribute(ShaderProgram *shaderProgram,char* attributeName, GLuint bufVBO, int vertexsize);

#endif
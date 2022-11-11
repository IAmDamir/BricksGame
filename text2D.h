#ifndef TEXT2D_H
#define TEXT2D_H

#include <vector>
#include <cstring>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shaderprogram.h"
#include "tools.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

void initText2D(char* texturePath);
void printText2D(const char* text, int x, int y, int size);
void cleanupText2D();

#endif
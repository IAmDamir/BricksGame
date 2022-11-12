#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "GL/glew.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

class ShaderProgram {
private:
	GLuint vertexShader;
	GLuint geometryShader;
	GLuint fragmentShader;
	char* readFile(char* fileName);
	GLuint loadShader(GLenum shaderType, char* fileName);
public:
	GLuint shaderProgram;
	ShaderProgram(char* vertexShaderFile, char* geometryShaderFile, char* fragmentShaderFile);
	ShaderProgram(const char* vertexShaderFile, char* geometryShaderFile, const char* fragmentShaderFile);
	~ShaderProgram();
	void use();
	GLuint getUniformLocation(char* variableName);
	GLuint getAttribLocation(char* variableName);
};

#endif
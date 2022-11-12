#define _CRT_SECURE_NO_DEPRECATE
#include "shaderprogram.h"

char* ShaderProgram::readFile(char* fileName) {
	int filesize;
	std::fstream plik(fileName);
	char* result;

	std::string str((std::istreambuf_iterator<char>(plik)),
		std::istreambuf_iterator<char>());
	filesize = str.size();
	result = new char[filesize + 1];
	strcpy(result, str.c_str());

	return result;
}

GLuint ShaderProgram::loadShader(GLenum shaderType, char* fileName) {
	GLuint shader = glCreateShader(shaderType);
	const GLchar* shaderSource = readFile(fileName);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	delete[]shaderSource;

	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 1) {
		infoLog = new char[infologLength];
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		delete[]infoLog;
	}

	return shader;
}

ShaderProgram::ShaderProgram(const char* vertexShaderFile, char* geometryShaderFile, const char* fragmentShaderFile)
{
	ShaderProgram(vertexShaderFile, geometryShaderFile, fragmentShaderFile);
}

ShaderProgram::ShaderProgram(char* vertexShaderFile, char* geometryShaderFile, char* fragmentShaderFile)
{
	printf("Loading vertex shader...\n");
	vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderFile);
	if (geometryShaderFile != NULL) {
		printf("Loading geometry shader...\n");
		geometryShader = loadShader(GL_GEOMETRY_SHADER, geometryShaderFile);
	}
	else {
		geometryShader = 0;
	}

	printf("Loading fragment shader...\n");
	fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderFile);

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	if (geometryShaderFile != NULL) glAttachShader(shaderProgram, geometryShader);
	glLinkProgram(shaderProgram);

	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 1)
	{
		infoLog = new char[infologLength];
		glGetProgramInfoLog(shaderProgram, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		delete[]infoLog;
	}

	printf("Shader program %s created \n", vertexShaderFile);
}

ShaderProgram::~ShaderProgram() {
	glDetachShader(shaderProgram, vertexShader);
	if (geometryShader != 0) glDetachShader(shaderProgram, geometryShader);
	glDetachShader(shaderProgram, fragmentShader);

	glDeleteShader(vertexShader);
	if (geometryShader != 0) glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	glDeleteProgram(shaderProgram);
}

void ShaderProgram::use() {
	glUseProgram(shaderProgram);
}

GLuint ShaderProgram::getUniformLocation(char* variableName) {
	return glGetUniformLocation(shaderProgram, variableName);
}

GLuint ShaderProgram::getAttribLocation(char* variableName) {
	return glGetAttribLocation(shaderProgram, variableName);
}
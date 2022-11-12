#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.h"
#include "gameobject.h"
#include "text2D.h"

void runGame(GLFWwindow* window);
void menuControl(GLFWwindow* window, int key, int scancode, int action, int mode);
void gameControl(GLFWwindow* window, int key, int scancode, int action, int mode);
void winControl(GLFWwindow* window, int key, int scancode, int action, int mode);
void loseControl(GLFWwindow* window, int key, int scancode, int action, int mode);
void freeOpenGLProgram();
void generateLevelBlocks();
void initOpenGLProgram();
void drawSceneAndDetectCollisions(GLFWwindow* window, float padDeltaX, float ballDeltaX[], float ballDeltaY[]);
void drawMenu(GLFWwindow* window);
void drawWIN(GLFWwindow* window);
void drawLOSE(GLFWwindow* window);
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
GLuint loadDDS(const char* imagepath);

enum gameState
{
	menu, game, win, lose
};

gameState state = menu;
bool pause = false;

const GLuint WIDTH = 1024, HEIGHT = 768;
const int ballCount = 3;
const int levelColumns = 7;
const int levelRows = 12;

ShaderProgram* shaderProgram;

float padVelocityX = 0.0f;
float ballVelocityX[ballCount];
float ballVelocityY[ballCount];

GameObject* leftWall;
GameObject* rightWall;
GameObject* upperWall;
GameObject* ground;
GameObject* pad;
GameObject* balls[ballCount];

GameObject* levelBlocks[levelColumns * levelRows];

glm::mat4 P = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.2f, 200.0f);
glm::mat4 V = glm::lookAt(
	glm::vec3(0.0f, -43.0f, -32.0f),
	glm::vec3(0.0f, -9.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
glm::vec3 lightpos0;
glm::vec3 lightpos1;

char cube_obj[] = "obj//cube.obj", sphere_obj[] = "obj//sphere.obj";
char vshader[] = "shaders//vshader.txt", fshader[] = "shaders//fshader.txt";
char depth_vshader[] = "shaders//DepthRTT.vertexshader", depth_fshader[] = "shaders//DepthRTT.fragmentshader";
char text[] = "res//text.png";
char metal_png[] = "res//metal.png", metal_spec_png[] = "res//metal_spec.png";
char titles_png[] = "res//tiles.png", titles_spec_png[] = "res//tiles_spec.png";
char plaster_png[] = "res//plaster.png", plaster_spec_png[] = "res//plaster_spec.png";
char boards_png[] = "res//boards.png", boards_spec_png[] = "res//boards_spec.png";
char brick_png[] = "res//brick.png", brick_spec_png[] = "res//brick_spec.png";

GLuint depthProgramID;
GLuint depthMatrixID;
GLuint FramebufferName;
// We would expect width and height to be 1024 and 768
int windowWidth = 1024;
int windowHeight = 768;
GLuint programID;
GLuint Texture;
GLuint TextureID;
GLuint texID;
std::vector<GameObject> objects;
GLuint depthTexture;
GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint DepthBiasID;
GLuint ShadowMapID;
GLuint lightInvDirID;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (state)
	{
	case menu:
		menuControl(window, key, scancode, action, mode);
		break;
	case game:
		gameControl(window, key, scancode, action, mode);
		break;
	case win:
		winControl(window, key, scancode, action, mode);
		break;
	case lose:
		loseControl(window, key, scancode, action, mode);
		break;
	}
}
void gameControl(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			return;
		}
		if (key == GLFW_KEY_LEFT)
		{
			padVelocityX = 30;
		}
		if (key == GLFW_KEY_RIGHT)
		{
			padVelocityX = -30;
		}
		if (key == GLFW_KEY_P)
		{
			pause = !pause;
		}
		if (key == GLFW_KEY_R)
		{
			initOpenGLProgram();
			glfwSetTime(0);
		}
	}
	if (action == GLFW_RELEASE)
	{
		padVelocityX = 0;
	}
}
void menuControl(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			return;
		}
		state = game;
		glfwSetTime(0);
	}
}
void winControl(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			return;
		}
		initOpenGLProgram();
		state = game;
		glfwSetTime(0);
	}
}
void loseControl(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			return;
		}
		initOpenGLProgram();
		state = game;
		glfwSetTime(0);
	}
}
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL-Arkanoid", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	initOpenGLProgram();

	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	programID = LoadShaders("shaders//ShadowMapping.vertexshader", "shaders//ShadowMapping.fragmentshader");
	texID = glGetUniformLocation(programID, "myTextureSampler");

	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
	ShadowMapID = glGetUniformLocation(programID, "shadowMap");

	lightInvDirID = glGetUniformLocation(programID, "LightInvDirection_worldspace");

	while (!glfwWindowShouldClose(window))
	{
		switch (state)
		{
		case menu:
			drawMenu(window);
			break;
		case game:
			runGame(window);
			break;
		case win:
			drawWIN(window);
			break;
		case lose:
			drawLOSE(window);
			break;
		}

		glfwPollEvents();
	}

	freeOpenGLProgram();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
void clearBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void initOpenGLProgram()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	initText2D(text);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	depthProgramID = LoadShaders(depth_vshader, depth_fshader);
	depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");

	Texture = loadDDS("res//uvmap.DDS");

	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	glDrawBuffer(GL_NONE);

	shaderProgram = new ShaderProgram(vshader, NULL, fshader);

	GameObjectVertices* sphere = new GameObjectVertices(sphere_obj);
	GameObjectVertices* cube = new GameObjectVertices(cube_obj);

	for (int i = 0; i < ballCount; i++)
	{
		ballVelocityX[i] = 0.0f;
		ballVelocityY[i] = 10.0f;

		balls[i] = new GameObject(sphere, readTexture(metal_png), readTexture(metal_spec_png), depthTexture,
			glm::vec3(-20.0f + 3.0f * i, -19.0f, 0.0f), glm::vec3(0.7f, 0.7f, 0.7f), V, P, shaderProgram);
		objects.push_back(*balls[i]);
	}

	GLuint titles = readTexture(titles_png);
	GLuint titlesSpec = readTexture(titles_spec_png);
	leftWall = new GameObject(cube, titles, titlesSpec, depthTexture,
		glm::vec3(25.0f, 0.0f, 0.0f), glm::vec3(1.0f, 50.0f, 1.0f), V, P, shaderProgram);
	rightWall = new GameObject(cube, titles, titlesSpec, depthTexture,
		glm::vec3(-25.0f, 0.0f, 0.0f), glm::vec3(1.0f, 50.0f, 1.0f), V, P, shaderProgram);
	upperWall = new GameObject(cube, titles, titlesSpec, depthTexture,
		glm::vec3(0.0f, 24.5f, 0.0f), glm::vec3(50.0f, 1.0f, 1.0f), V, P, shaderProgram);
	ground = new GameObject(cube, readTexture(plaster_png), readTexture(plaster_spec_png), depthTexture,
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(51.0f, 50.0f, 1.0f), V, P, shaderProgram);
	pad = new GameObject(cube, readTexture(boards_png), readTexture(boards_spec_png), depthTexture,
		glm::vec3(0.0f, -24.5f, 0.0f), glm::vec3(6.0f, 1.0f, 1.0f), V, P, shaderProgram);

	objects.push_back(*leftWall);
	objects.push_back(*rightWall);
	objects.push_back(*upperWall);
	objects.push_back(*ground);
	objects.push_back(*pad);

	generateLevelBlocks();
}
void generateLevelBlocks()
{
	GameObjectVertices* cube = new GameObjectVertices(cube_obj);
	GLuint metal = readTexture(brick_png);
	GLuint metalSpec = readTexture(brick_spec_png);

	for (int i = 0; i < levelRows; i++)
	{
		for (int j = 0; j < levelColumns; j++)
		{
			GameObject* levelBlock = new GameObject(cube, metal, metalSpec, depthTexture,
				glm::vec3(21.0f - 7.0f * j, 23.8f - 2.0f * i, 0.0f), glm::vec3(6.5f, 1.0f, 1.0f), V, P, shaderProgram);
			if (i % 2 == 0 && j % 2 != 0)
				levelBlock->show = false;
			levelBlocks[levelColumns * i + j] = levelBlock;
			objects.push_back(*levelBlock);
		}
	}
}
void runGame(GLFWwindow* window)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0, 0, 1024, 1024);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(depthProgramID);

	glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);

	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// or, for spot light :
	//glm::vec3 lightPos(5, 20, 20);
	//glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
	//glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));

	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

	glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

	float FoV = 45.0f;
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);;
	glm::vec3 position = glm::vec3(0, 0, 5);
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	float horizontalAngle = 0.005f * float(1024 / 2 - xpos);
	float verticalAngle = 0.005f * float(768 / 2 - ypos);

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	glm::vec3 up = glm::cross(right, direction);
	glm::mat4 ViewMatrix = glm::lookAt(
		position,
		position + direction,
		up
	);

	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

	for (auto& obj : objects)
	{
		//vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, obj.bufVertices);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.bufIndeces);

		glDrawElements(
			GL_TRIANGLES,
			obj.indices.size(),
			GL_UNSIGNED_SHORT,
			(void*)0
		);
	}

	glDisableVertexAttribArray(0);

	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

	glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	glUniform1i(TextureID, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(ShadowMapID, 1);

	for (auto& obj : objects)
	{
		//vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, obj.bufVertices);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		//UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, obj.bufTexCoords);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		//normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, obj.bufNormals);
		glVertexAttribPointer(
			2,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.bufIndeces);

		glDrawElements(
			GL_TRIANGLES,
			obj.indices.size(),
			GL_UNSIGNED_SHORT,
			(void*)0
		);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glDisableVertexAttribArray(0);

	clearBuffer();

	float padDeltaX = glfwGetTime() * padVelocityX;
	if (pause)
		padDeltaX = 0.0f;
	float ballDeltaX[ballCount];
	float ballDeltaY[ballCount];

	for (int i = 0; i < ballCount; i++)
	{
		ballDeltaX[i] = glfwGetTime() * ballVelocityX[i];
		ballDeltaY[i] = glfwGetTime() * ballVelocityY[i];
		if (pause)
		{
			ballDeltaX[i] = 0.0f;
			ballDeltaY[i] = 0.0f;
		}
	}

	glfwSetTime(0);

	drawSceneAndDetectCollisions(window, padDeltaX, ballDeltaX, ballDeltaY);
}
void drawSceneAndDetectCollisions(GLFWwindow* window, float padDeltaX, float ballDeltaX[], float ballDeltaY[])
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pad->MoveHorizontal(padDeltaX);

	if (pad->leftEdgeX > leftWall->rightEdgeX || pad->rightEdgeX < rightWall->leftEdgeX)
	{
		pad->MoveHorizontal(-padDeltaX);
	}

	for (int i = 0; i < ballCount; i++)
	{
		if (balls[i]->show)
		{
			balls[i]->MoveHorizontal(ballDeltaX[i]);
			balls[i]->MoveVertical(ballDeltaY[i]);

			if (balls[i]->upperEdgeY < pad->bottomEdgeY)
			{
				balls[i]->show = false;
				continue;
			}
			if (balls[i]->leftEdgeX > leftWall->rightEdgeX || balls[i]->rightEdgeX < rightWall->leftEdgeX)
			{
				ballVelocityX[i] = -ballVelocityX[i];
				balls[i]->MoveHorizontal(-ballDeltaX[i]);
			}
			if (balls[i]->upperEdgeY > upperWall->bottomEdgeY)
			{
				ballVelocityY[i] = -ballVelocityY[i];
				balls[i]->MoveVertical(-ballDeltaY[i]);
			}
			if (balls[i]->rightEdgeX < pad->leftEdgeX && balls[i]->leftEdgeX > pad->rightEdgeX &&
				balls[i]->bottomEdgeY < pad->upperEdgeY && balls[i]->upperEdgeY > pad->upperEdgeY)
			{
				float ballMiddle = (balls[i]->leftEdgeX + balls[i]->rightEdgeX) / 2;
				float factor = ((ballMiddle - pad->rightEdgeX) / (pad->leftEdgeX - pad->rightEdgeX)) * 2 - 1;
				factor = roundf(factor * 100) / 100;

				ballVelocityY[i] = -ballVelocityY[i];
				ballVelocityX[i] = ballVelocityY[i] * factor;

				balls[i]->MoveVertical(-ballDeltaY[i]);
			}

			for (int j = 0; j < levelColumns * levelRows; j++)
			{
				if (levelBlocks[j]->show)
				{
					if ((balls[i]->bottomEdgeY < levelBlocks[j]->upperEdgeY && balls[i]->upperEdgeY > levelBlocks[j]->upperEdgeY) ||
						(balls[i]->upperEdgeY > levelBlocks[j]->bottomEdgeY && balls[i]->bottomEdgeY < levelBlocks[j]->bottomEdgeY))
					{
						if (balls[i]->leftEdgeX < levelBlocks[j]->leftEdgeX && balls[i]->rightEdgeX > levelBlocks[j]->rightEdgeX)
						{
							ballVelocityY[i] = -ballVelocityY[i];
							balls[i]->MoveVertical(-ballDeltaY[i]);
							levelBlocks[j]->show = false;
							break;
						}
						if (balls[i]->rightEdgeX < levelBlocks[j]->leftEdgeX && balls[i]->leftEdgeX > levelBlocks[j]->rightEdgeX)
						{
							if (ballVelocityX[i] < 2)
							{
								ballVelocityY[i] = -ballVelocityY[i];
								balls[i]->MoveVertical(-ballDeltaY[i]);
							}
							else
							{
								ballVelocityX[i] = -ballVelocityX[i];
								balls[i]->MoveHorizontal(-ballDeltaX[i]);
							}
							levelBlocks[j]->show = false;
							break;
						}
					}
				}
			}
		}
	}
	/*
	lightpos0 = pad->GetPosition();
	lightpos1 = balls[0]->GetPosition();
	*/
	bool isAnyBall = false;
	for (int i = 0; i < ballCount; i++)
	{
		if (balls[i]->show)
		{
			/*
			balls[i]->SetLigthPosition0(lightpos0);
			balls[i]->SetLigthPosition1(lightpos1);
			*/
			balls[i]->DrawObject();
			isAnyBall = true;
		}
	}
	if (!isAnyBall)
		state = lose;
	/*
	pad->SetLigthPosition0(lightpos0);
	pad->SetLigthPosition1(lightpos1);
	upperWall->SetLigthPosition0(lightpos0);
	upperWall->SetLigthPosition1(lightpos1);
	leftWall->SetLigthPosition0(lightpos0);
	leftWall->SetLigthPosition1(lightpos1);
	rightWall->SetLigthPosition0(lightpos0);
	rightWall->SetLigthPosition1(lightpos1);
	ground->SetLigthPosition0(lightpos0);
	ground->SetLigthPosition1(lightpos1);
	*/
	pad->DrawObject();
	upperWall->DrawObject();
	leftWall->DrawObject();
	rightWall->DrawObject();
	ground->DrawObject();

	bool isAnyBlock = false;
	for (int i = 0; i < levelColumns * levelRows; i++)
	{
		if (levelBlocks[i]->show)
		{
			//levelBlocks[i]->SetLigthPosition(lightpos);
			levelBlocks[i]->DrawObject();
			isAnyBlock = true;
		}
	}
	if (!isAnyBlock)
		state = win;

	if (pause)
		printText2D("PAUSE", 250, 300, 60);

	printText2D("esc - exit", 10, 570, 20);
	printText2D("p - pause", 10, 550, 20);
	printText2D("r - restart", 10, 530, 20);

	glfwSwapBuffers(window);
}
void drawMenu(GLFWwindow* window)
{
	clearBuffer();

	printText2D("Press any key to start ...", 130, 300, 20);

	glfwSwapBuffers(window);
}
void drawWIN(GLFWwindow* window)
{
	clearBuffer();

	printText2D("You win !", 100, 300, 60);
	printText2D("Press any key to restart or esc to exit", 100, 250, 15);

	glfwSwapBuffers(window);
}
void drawLOSE(GLFWwindow* window)
{
	clearBuffer();

	printText2D("You lose !", 100, 300, 60);
	printText2D("Press any key to restart or esc to exit", 100, 250, 15);

	glfwSwapBuffers(window);
}

void freeOpenGLProgram()
{
	delete shaderProgram;
	delete leftWall;
	delete rightWall;
	delete upperWall;
	delete ground;
	delete pad;
	for (int i = 0; i < ballCount; i++)
	{
		delete balls[i];
	}
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char* imagepath)
{
	unsigned char header[124];

	FILE* fp;

	fp = fopen(imagepath, "rb");
	if (fp == NULL) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
		return 0;
	}

	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return 0;
	}

	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	unsigned char* buffer;
	unsigned int bufsize;

	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);

	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		if (width < 1) width = 1;
		if (height < 1) height = 1;
	}

	free(buffer);

	return textureID;
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		return 0;
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
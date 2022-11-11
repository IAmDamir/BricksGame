#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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

	//glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetKeyCallback(window, key_callback);
	initOpenGLProgram();

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
void initOpenGLProgram()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	char temp[] = "text.png";
	initText2D(temp);

	shaderProgram = new ShaderProgram("vshader.txt", NULL, "fshader.txt");

	GameObjectVertices* cube = new GameObjectVertices("cube.obj");
	GameObjectVertices* sphere = new GameObjectVertices("sphere.obj");

	for (int i = 0; i < ballCount; i++)
	{
		ballVelocityX[i] = 0.0f;
		ballVelocityY[i] = 10.0f;

		balls[i] = new GameObject(sphere, readTexture("metal.png"), readTexture("metal_spec.png"),
			glm::vec3(-20.0f + 3.0f * i, -19.0f, 0.0f), glm::vec3(0.7f, 0.7f, 0.7f), V, P, shaderProgram);
	}

	GLuint titles = readTexture("tiles.png");
	GLuint titlesSpec = readTexture("tiles_spec.png");
	leftWall = new GameObject(cube, titles, titlesSpec,
		glm::vec3(25.0f, 0.0f, 0.0f), glm::vec3(1.0f, 50.0f, 1.0f), V, P, shaderProgram);
	rightWall = new GameObject(cube, titles, titlesSpec,
		glm::vec3(-25.0f, 0.0f, 0.0f), glm::vec3(1.0f, 50.0f, 1.0f), V, P, shaderProgram);
	upperWall = new GameObject(cube, titles, titlesSpec,
		glm::vec3(0.0f, 24.5f, 0.0f), glm::vec3(50.0f, 1.0f, 1.0f), V, P, shaderProgram);
	ground = new GameObject(cube, readTexture("plaster.png"), readTexture("plaster_spec.png"),
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(51.0f, 50.0f, 1.0f), V, P, shaderProgram);
	pad = new GameObject(cube, readTexture("boards.png"), readTexture("boards_spec.png"),
		glm::vec3(0.0f, -24.5f, 0.0f), glm::vec3(6.0f, 1.0f, 1.0f), V, P, shaderProgram);

	generateLevelBlocks();
}
void generateLevelBlocks()
{
	GameObjectVertices* cube = new GameObjectVertices("cube.obj");
	GLuint metal = readTexture("brick.png");
	GLuint metalSpec = readTexture("brick_spec.png");

	for (int i = 0; i < levelRows; i++)
	{
		for (int j = 0; j < levelColumns; j++)
		{
			GameObject* levelBlock = new GameObject(cube, metal, metalSpec,
				glm::vec3(21.0f - 7.0f * j, 23.8f - 2.0f * i, 0.0f), glm::vec3(6.5f, 1.0f, 1.0f), V, P, shaderProgram);
			if (i % 2 == 0 && j % 2 != 0)
				levelBlock->show = false;
			levelBlocks[levelColumns * i + j] = levelBlock;
		}
	}
}
void runGame(GLFWwindow* window)
{
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

	bool isAnyBall = false;
	for (int i = 0; i < ballCount; i++)
	{
		if (balls[i]->show)
		{
			balls[i]->DrawObject();
			isAnyBall = true;
		}
	}
	if (!isAnyBall)
		state = lose;

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	printText2D("Press any key to start ...", 130, 300, 20);

	glfwSwapBuffers(window);
}
void drawWIN(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	printText2D("You win !", 100, 300, 60);
	printText2D("Press any key to restart or esc to exit", 100, 250, 15);

	glfwSwapBuffers(window);
}
void drawLOSE(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
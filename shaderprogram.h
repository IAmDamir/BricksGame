/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "GL/glew.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

class ShaderProgram {
private:
	GLuint vertexShader; //Uchwyt reprezentujący vertex shader
	GLuint geometryShader; //Uchwyt reprezentujący geometry shader
	GLuint fragmentShader; //Uchwyt reprezentujący fragment shader
	char* readFile(char* fileName); //metoda wczytująca plik tekstowy do tablicy znaków
	GLuint loadShader(GLenum shaderType, char* fileName); //Metoda wczytuje i kompiluje shader, a następnie zwraca jego uchwyt
public:
	GLuint shaderProgram; //Uchwyt reprezentujący program cieniujacy
	ShaderProgram(char* vertexShaderFile, char* geometryShaderFile, char* fragmentShaderFile);
	~ShaderProgram();
	void use(); //Włącza wykorzystywanie programu cieniującego
	GLuint getUniformLocation(char* variableName); //Pobiera numer slotu związanego z daną zmienną jednorodną
	GLuint getAttribLocation(char* variableName); //Pobiera numer slotu związanego z danym atrybutem
};

#endif
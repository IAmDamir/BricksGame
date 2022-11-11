#include "text2D.h"

unsigned int Text2DTextureID;
unsigned int Text2DVertexBufferID;
unsigned int Text2DUVBufferID;
ShaderProgram* Text2DShader;
unsigned int Text2DUniformID;

void initText2D(char* texturePath) {
	// Initialize texture
	Text2DTextureID = readTexture(texturePath);

	// Initialize VBO
	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);

	// Initialize Shader
	char vshader[] = "shaders//textvshader.txt", fshader[] = "shaders//textfshader.txt";
	Text2DShader = new ShaderProgram(vshader, NULL, fshader);

	// Initialize uniforms' IDs
	char sampler[] = "myTextureSampler";
	Text2DUniformID = Text2DShader->getUniformLocation(sampler);
}

void printText2D(const char* text, int x, int y, int size) {
	unsigned int length = strlen(text);

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for (unsigned int i = 0; i < length; i++) {
		glm::vec2 vertex_up_left = glm::vec2(x + i * size, y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i * size + size, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i * size + size, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i * size, y);

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character % 16) / 16.0f;
		float uv_y = (character / 16) / 16.0f;

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left);
		UVs.push_back(uv_up_right);

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}

	GLuint bufVertices = makeBuffer(&vertices[0], vertices.size(), sizeof(float) * 2);
	GLuint bufTexCoords = makeBuffer(&UVs[0], UVs.size(), sizeof(float) * 2);
	GLuint vao;
	glGenVertexArrays(1, &vao);

	Text2DShader->use();

	glBindVertexArray(vao);

	char position_screenspace[] = "vertexPosition_screenspace", vertexUV[] = "vertexUV";
	assignVBOtoAttribute(Text2DShader, position_screenspace, bufVertices, 2);
	assignVBOtoAttribute(Text2DShader, vertexUV, bufTexCoords, 2);

	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
	glUniform1i(Text2DUniformID, 0);

	glBindVertexArray(vao);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glDisable(GL_BLEND);

	glBindVertexArray(0);
}

void cleanupText2D() {
	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Text2DTextureID);

	// Delete shader
	//glDeleteProgram(Text2DShaderID);
	delete Text2DShader;
}
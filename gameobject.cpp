#include "gameobject.h"

GameObject::GameObject(GameObjectVertices* gameObjectVertices, GLuint texture0, GLuint texture1,
	glm::vec3 position, glm::vec3 scale, glm::mat4 view, glm::mat4 perspective, ShaderProgram* shaderProgram)
{
	show = true;

	GameObject::position = position;
	GameObject::scale = scale;

	CalculateModel();

	GameObject::view = view;
	GameObject::perspective = perspective;
	GameObject::shaderProgram = shaderProgram;

	vertices = gameObjectVertices->vertices;
	texcoords = gameObjectVertices->texcoords;
	normals = gameObjectVertices->normals;

	bufVertices = gameObjectVertices->bufVertices;
	bufNormals = gameObjectVertices->bufNormals;
	bufTexCoords = gameObjectVertices->bufTexCoords;

	vao = gameObjectVertices->vao;
	indices = gameObjectVertices->indices;
	bufIndeces = gameObjectVertices->bufIndeces;

	GameObject::texture0 = texture0;
	GameObject::texture1 = texture1;

	CalculateEdges();
}

void GameObject::CalculateModel()
{
	model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
}

void GameObject::CalculateEdges()
{
	leftEdgeX = (model * glm::vec4(vertices[0].x, vertices[0].y, vertices[0].z, 1.0f)).x;
	rightEdgeX = (model * glm::vec4(vertices[0].x, vertices[0].y, vertices[0].z, 1.0f)).x;
	upperEdgeY = (model * glm::vec4(vertices[0].x, vertices[0].y, vertices[0].z, 1.0f)).y;
	bottomEdgeY = (model * glm::vec4(vertices[0].x, vertices[0].y, vertices[0].z, 1.0f)).y;

	for (int i = 0; i < vertices.size(); i++)
	{
		glm::vec4 vertex = glm::vec4(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
		glm::vec4 position = model * vertex;

		if (position.x > leftEdgeX)
			leftEdgeX = position.x;
		if (position.x < rightEdgeX)
			rightEdgeX = position.x;
		if (position.y > upperEdgeY)
			upperEdgeY = position.y;
		if (position.y < bottomEdgeY)
			bottomEdgeY = position.y;
	}
}

void GameObject::DrawObject()
{
	CalculateModel();

	shaderProgram->use();

	glBindVertexArray(vao);

	char position[] = "position", normal[] = "normal", texCoord0[] = "texCoord0";
	assignVBOtoAttribute(shaderProgram, position, bufVertices, 3);
	assignVBOtoAttribute(shaderProgram, normal, bufNormals, 3);
	assignVBOtoAttribute(shaderProgram, texCoord0, bufTexCoords, 2);

	glBindVertexArray(0);

	char P[] = "P", M[] = "M", V[] = "V";
	char lightPos0[] = "lightPos0", lightPos1[] = "lightPos1";
	char textureMap0[] = "textureMap0", textureMap1[] = "textureMap1";

	glUniformMatrix4fv(shaderProgram->getUniformLocation(P), 1, false, glm::value_ptr(perspective));
	glUniformMatrix4fv(shaderProgram->getUniformLocation(V), 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(shaderProgram->getUniformLocation(M), 1, false, glm::value_ptr(model));
	//glUniformMatrix4fv(shaderProgram->getUniformLocation(lightPos0), 1, false, glm::value_ptr(lightpos));
	glUniform4f(shaderProgram->getUniformLocation(lightPos0), 0.0f, 12.0f, -5.0f, 1);
	glUniform4f(shaderProgram->getUniformLocation(lightPos1), 0.0f, -26.0f, -3.0f, 1);
	glUniform1i(shaderProgram->getUniformLocation(textureMap0), 0);
	glUniform1i(shaderProgram->getUniformLocation(textureMap1), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);
}

void GameObject::MoveHorizontal(float delta)
{
	position.x += delta;
	leftEdgeX += delta;
	rightEdgeX += delta;
}

void GameObject::MoveVertical(float delta)
{
	position.y += delta;
	upperEdgeY += delta;
	bottomEdgeY += delta;
}
/*
glm::vec3 GameObject::GetPosition()
{
	return position;
}

void GameObject::SetLigthPosition(glm::vec3 lightPos)
{
	this->lightpos = glm::mat4(1.0f);
	this->lightpos = glm::translate(lightpos, lightPos);
}*/
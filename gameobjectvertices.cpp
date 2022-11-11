#include "gameobjectvertices.h"

GameObjectVertices::GameObjectVertices(char* objFileName)
{
	loadOBJ(objFileName, vertices, texcoords, normals);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		unsigned short newindex = (unsigned short)vertices.size() - 1;
		indices.push_back(newindex);
	}

	bufVertices = makeBuffer(&vertices[0], vertices.size(), sizeof(float) * 3);
	bufNormals = makeBuffer(&normals[0], normals.size(), sizeof(float) * 3);
	bufTexCoords = makeBuffer(&texcoords[0], texcoords.size(), sizeof(float) * 2);
	// Generate a buffer for the indices as well
	glGenBuffers(1, &bufIndeces);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIndeces);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
}
#include "MeshResource.h"

#include <string>
#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

void MeshResource::GetId(uint &index, uint &vertex, uint &normal, uint &uv) const
{
	index = id_index;
	vertex = id_vertex;
	normal = id_normals;
	uv = id_uvs;
}

void MeshResource::LoadBuffers()
{

	glGenBuffers(1, (GLuint*) &(id_vertex));

	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(id_index));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_index, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (num_uvs > 0)
	{
		glGenBuffers(1, (GLuint*) &(id_uvs));

		glBindBuffer(GL_ARRAY_BUFFER, id_uvs);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 2, uvs, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (num_normals > 0)
	{

		glGenBuffers(1, (GLuint*) &(id_normals));

		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
}

void MeshResource::Clear()
{
	if (indices != nullptr)
		delete[] indices;

	if (vertices != nullptr)
		delete[] vertices;

	if (uvs != nullptr)
		delete[] uvs;

	if (normals != nullptr)
		delete[] normals;
}

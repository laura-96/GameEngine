#include "MeshResource.h"


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

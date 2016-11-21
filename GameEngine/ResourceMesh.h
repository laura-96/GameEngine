#ifndef __ResourceMesh_H__
#define __ResourceMesh_H__

#include "Resource.h"

class RecourceMesh : public Resource
{
public:

	ResourceMesh(uint mesh_uid) : Resource(Resource::ResourceType::Mesh, mesh_uid) {};

	void Clear();

public:

	uint* indices = nullptr;
	float* vertices = nullptr;
	float* normals = nullptr;
	float* uvs = nullptr;

	uint num_index = 0;
	uint num_vertex = 0;
	uint num_normals = 0;
	uint num_uvs = 0;

	uint id_index = -1;
	uint id_vertex = -1;
	uint id_normals = -1;
	uint id_uvs = -1;

	int index_material = -1;

};

#endif // __ResourceMesh_H__

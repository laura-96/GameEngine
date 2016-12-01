#ifndef __MeshResource_H__
#define __MeshResource_H__


#include "Resource.h"

class MeshResource : public Resource
{

public:
	MeshResource(uint mesh_uid, const char* archive) : Resource(ResourceType::Mesh, mesh_uid, archive) {};

	void LoadBuffers();
	void GetId(uint &index, uint &vertex, uint &normal, uint &uv) const;
	void Clear();


	uint* indices = nullptr;
	float* vertices = nullptr;
	float* normals = nullptr;
	float* uvs = nullptr;

	uint num_index = 0;
	uint num_vertex = 0;
	uint num_normals = 0;
	uint num_uvs = 0;

private:
	uint id_index = -1;
	uint id_vertex = -1;
	uint id_normals = -1;
	uint id_uvs = -1;

	int index_material = -1;
};

#endif

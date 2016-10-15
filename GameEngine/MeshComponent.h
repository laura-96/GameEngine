#ifndef __MeshComponent_H__
#define __MeshComponent_H__

#include "Component.h"
#include "Globals.h"

class MeshComponent : public Component
{

public:

	MeshComponent(GameObject* _GO_belong): Component(ComponentType::Mesh, _GO_belong){};

	void Draw();
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

#endif //__MeshComponent_H__

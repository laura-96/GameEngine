#ifndef __MeshComponent_H__
#define __MeshComponent_H__

#include "Component.h"
#include "Globals.h"

class MeshComponent : public Component
{

public:

	MeshComponent(GameObject* _GO_belong): Component(ComponentType::Mesh, _GO_belong){};


public:

	uint* indices = nullptr;
	float* vertices = nullptr;

	uint num_index = 0;
	uint num_vertex = 0;
	uint num_uvs = 0;

	uint id_index = -1;
	uint id_vertex = -1;
	uint uvs = -1;
};

#endif //__MeshComponent

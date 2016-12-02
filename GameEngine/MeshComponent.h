#ifndef __MeshComponent_H__
#define __MeshComponent_H__

#include "Component.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class MeshResource;

class MeshComponent : public Component
{

public:

	MeshComponent(GameObject* _GO_belong): Component(ComponentType::Mesh, _GO_belong){};
	MeshComponent(GameObject* _GO_belong, MeshResource* mesh_resource);
	void SetResource(MeshResource* new_mesh_res);
	void Clear();

	math::AABB mesh_box;
	MeshResource* mesh_res;

};

#endif //__MeshComponent_H__

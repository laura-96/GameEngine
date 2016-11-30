#include "MeshComponent.h"

#include "GameObject.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"

#include "glmath.h"

#include "Geometry.h"
#include "MeshResource.h"

void MeshComponent::Clear()
{

	delete this;
}


MeshComponent::MeshComponent(GameObject* _GO_belong, MeshResource* mesh_resource)
{
	Component(ComponentType::Mesh, _GO_belong);
	mesh_res = mesh_resource;
}


void MeshComponent::SetResource(MeshResource* new_mesh_res)
{
	mesh_res = new_mesh_res;
}

	

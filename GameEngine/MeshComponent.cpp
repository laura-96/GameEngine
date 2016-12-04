#include "MeshComponent.h"

#include "GameObject.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"

#include "glmath.h"

#include "Geometry.h"
#include "MeshResource.h"

void MeshComponent::Clear()
{
	std::vector<GameObject*>::iterator go_res = mesh_res->gos_related.begin();
	while (go_res != mesh_res->gos_related.end())
	{
		if ((*go_res) == this->GO_belong)
		{
			mesh_res->gos_related.erase(go_res);
			break;
		}
		go_res++;
	}

	mesh_res = nullptr;
	delete this;
}


MeshComponent::MeshComponent(GameObject* _GO_belong, MeshResource* mesh_resource)
{
	Component(ComponentType::Mesh, _GO_belong);
	mesh_res = mesh_resource;
	//mesh_box.SetNegativeInfinity();
	mesh_box.Enclose((math::float3*)mesh_resource->vertices, mesh_resource->num_vertex);
}


void MeshComponent::SetResource(MeshResource* new_mesh_res)
{
	mesh_res = new_mesh_res;
	//mesh_box.SetNegativeInfinity();
	mesh_box.Enclose((math::float3*)new_mesh_res->vertices, new_mesh_res->num_vertex);

}

	

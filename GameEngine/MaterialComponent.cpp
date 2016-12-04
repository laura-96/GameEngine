#include "MaterialComponent.h"

#include "MaterialResource.h"


MaterialComponent::MaterialComponent(GameObject* _GO_belong, MaterialResource* mat_resource)
{
	Component(ComponentType::Material, _GO_belong);
	material_res = mat_resource;
}


void MaterialComponent::SetResource(MaterialResource* new_mat_res)
{
	material_res = new_mat_res;
}

uint MaterialComponent::GetTexture() const
{
	return material_res->texture[0];
}

void MaterialComponent::Clear()
{
	delete this;
}
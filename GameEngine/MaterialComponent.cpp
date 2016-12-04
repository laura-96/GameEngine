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
	std::vector<GameObject*>::iterator go_res = material_res->gos_related.begin();
	while (go_res != material_res->gos_related.end())
	{
		if ((*go_res) == this->GO_belong)
		{
			material_res->gos_related.erase(go_res);
			break;
		}
		go_res++;
	}

	material_res = nullptr;
	delete this;
}
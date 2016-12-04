#ifndef __MaterialComponent_H__
#define __MaterialComponent_H__

#include "Component.h"
#include "Globals.h"
#include "Color.h"

#include <string>

class MaterialResource;

class MaterialComponent : public Component
{

public:

	MaterialComponent(GameObject* _GO_belong) : Component(ComponentType::Material, _GO_belong) {};
	MaterialComponent(GameObject* _GO_belong, MaterialResource* mat_resource);

	uint GetTexture() const;

	void SetResource(MaterialResource* new_mat_res);
	void Clear();

private:

	MaterialResource* material_res;

};

#endif //__MaterialComponent_H__

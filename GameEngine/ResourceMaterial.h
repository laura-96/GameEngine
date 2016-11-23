#ifndef __MaterialResource_H__
#define __MaterialResource_H__

#include "Component.h"
#include "Globals.h"
#include "Color.h"

#include <string>

class MaterialComponent : public Component
{

public:

	MaterialComponent(GameObject* _GO_belong) : Component(ComponentType::Material, _GO_belong) {};

	void Clear();

public:

	uint material_id = -1;

	uint id_image = -1;
	uint texture[1];
	std::string path;

};

#endif //__MaterialResource_H__

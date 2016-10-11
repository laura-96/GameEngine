#ifndef __MaterialComponent_H__
#define __MaterialComponent_H__

#include "Component.h"
#include "Globals.h"

#include <string>

class MaterialComponent : public Component
{

public:

	MaterialComponent(GameObject* _GO_belong) : Component(ComponentType::Material, _GO_belong) {};

	//void Draw();

public:

	uint material_id = -1;

	uint id_image = -1;
	uint texture[1];
	std::string path;
};

#endif //__MaterialComponent_H__

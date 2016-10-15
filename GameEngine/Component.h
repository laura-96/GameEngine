#ifndef __Component_H__
#define __Component_H__

#include "Globals.h"

class GameObject;
class MeshComponent;
class MaterialComponent;
class TransformComponent;

class Component 
{
public:
	enum ComponentType
	{
		Mesh,
		Transform,
		Material,
		None
	};

	Component();
	Component(ComponentType type, GameObject* _GO_belong);
	ComponentType GetType() const;
	
	bool Enable(bool en_dis_able);
	const GameObject* GetGO() const;

	virtual void Clear();

public:
	bool enable = true;
	ComponentType comp_type = ComponentType::None;
	GameObject* GO_belong = nullptr;
};

#endif //__Component_H__

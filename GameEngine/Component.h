#ifndef __Component_H__
#define __Component_H__

class GameObject;
class MeshComponent;

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
	virtual void Draw() {};

public:
	ComponentType comp_type = None;
	GameObject* GO_belong = nullptr;
};

#endif //__Component_H__

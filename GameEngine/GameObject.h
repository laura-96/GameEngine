#ifndef __GameObject_H__
#define __GameObject_H__

#include <vector>
#include <list>
#include <string>

#include "Component.h"
#include "Globals.h"

class GameObject
{
public:

	GameObject(GameObject* _parent, const char* _name);

	MeshComponent* CreateMeshComponent();
	MaterialComponent* CreateMaterialComponent();
	TransformComponent* CreateTransformComponent();

	Component* FindComponent(const Component::ComponentType _type) const;
	const char* GetName() const;

	void Update();
	void Draw() const;
	void Clear(); //Cleans up GameObject

public:

	std::vector<Component*> components;
	std::list<GameObject*> children;

	GameObject* GO_parent = nullptr;
	std::string name;

	bool selected = false;
	bool active = true;
};

#endif //__GameObject_H__

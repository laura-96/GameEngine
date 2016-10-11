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

	Component* FindComponent(const Component::ComponentType _type) const;
	const char* GetName() const;

	void Update();

	std::vector<uint> id_tree; //Used to know the position of a game object in the tree
	
public:

	std::vector<Component*> components;
public:
	std::list<GameObject*> children;

	GameObject* GO_parent = nullptr;
	std::string name;
	bool active = true;
};

#endif //__GameObject_H__

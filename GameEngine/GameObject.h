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

	GameObject* GetChildAt(uint index) const;

	MeshComponent* CreateMeshComponent();

	Component* FindComponent(Component::ComponentType _type) const;
	const char* GetName() const;

	//void Update();

	std::vector<uint> id_tree; //Used to know the position of a game object in the tree
	
private:

	std::vector<Component*> components;
	std::list<GameObject*> children;

	GameObject* GO_parent = nullptr;
	std::string name;
	bool active = true;
};

#endif //__GameObject_H__

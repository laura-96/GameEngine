#include "GameObject.h"

#include "MeshComponent.h"

using namespace std;


GameObject::GameObject(GameObject* _parent, const char* _name)
{
	GO_parent = _parent;
	name = _name;
	
	if (GO_parent != nullptr)
	{
		GO_parent->children.push_back(this);
	}
}

GameObject* GameObject::GetChildAt(uint index) const
{
	GameObject* ret = nullptr;

	if (!children.empty() && index <= (children.size() - 1))
	{
		list<GameObject*>::const_iterator it = children.begin();
		
		uint i = 1;
		while(i <= index)
		{
			it++;
			i++;
		}
		ret = (*it);
	}

	return ret;
}


MeshComponent* GameObject::CreateMeshComponent()
{
	MeshComponent* ret = new MeshComponent((GameObject*)this);
	
	Component* comp = ret;
	components.push_back(comp);

	return ret;
}

//Doesn't enter FindComponent
Component* GameObject::FindComponent(Component::ComponentType _type) const
{
	Component* ret = nullptr;

	vector<Component*>::const_iterator it = components.begin();

	for ( ; (*it)->GetType() != _type && it != components.end(); it++)
	{
		if ((*it)->GetType() == _type)
		{
			ret = (*it);
		}
	}
	
	return ret;
}

const char* GameObject::GetName() const
{
	return (name.c_str());
}

void GameObject::Update()
{
	if (GO_parent != nullptr)
	{
		if (GO_parent->active != active)
		{
			active = GO_parent->active;
		}
	}

	if (active)
	{
		if (components.size() > 0)
		{
			vector<Component*>::iterator m = components.begin();//= FindComponent(Component::ComponentType::Mesh);
			(*m)->Draw();
		}
		
	}
}
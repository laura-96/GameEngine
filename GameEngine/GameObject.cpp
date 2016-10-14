#include "GameObject.h"

#include "Component.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"

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


MeshComponent* GameObject::CreateMeshComponent()
{
	MeshComponent* ret = new MeshComponent((GameObject*)this);
	
	Component* comp = ret;
	components.push_back(comp);

	return ret;
}

MaterialComponent* GameObject::CreateMaterialComponent()
{
	MaterialComponent* ret = new MaterialComponent((GameObject*)this);

	Component* comp = ret;
	components.push_back(comp);

	return ret;
}

TransformComponent* GameObject::CreateTransformComponent()
{
	TransformComponent* ret = new TransformComponent((GameObject*)this);

	Component* comp = ret;
	components.push_back(comp);

	return ret;
}

Component* GameObject::FindComponent(const Component::ComponentType _type) const
{
	Component* ret = nullptr;
	if (!components.empty())
	{
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->comp_type == _type)
			{
				ret = components[i];
				break;
			}
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
			while(m != components.end())
			{
				(*m)->Draw();
				m++;
			}
			
		}

		if (!children.empty())
		{
			for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
			{
				(*it)->Update();
			}
		}
	}
}

void GameObject::Clear()
{
	name.clear();
	active = false;

	if (!children.empty())
	{
		children.clear();
	}

	for (uint i = 0; i < components.size(); i++)
	{
		RELEASE(components[i]);
	}
	components.clear();

	delete (this);
}
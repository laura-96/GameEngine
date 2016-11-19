#ifndef __Resource_H__
#define __Resource_H__

#include "Globals.h"
#include <vector>

class GameObject;

class Resource
{

public:
	enum ResourceType
	{
		Mesh,
		Transform,
		Material,
		Prefab,
		None
	};

	Resource(ResourceType type);
	ResourceType GetType() const;

	uint Count(std::vector<GameObject*> &go_using_res) const;

	virtual void Clear();

private:
	std::vector<GameObject*> go_related;
	ResourceType res_type = ResourceType::None;
	char* archive;
};

#endif
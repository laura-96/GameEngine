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

	Resource(ResourceType type, uint resource_uid);
	ResourceType GetType() const;

	uint Count(std::vector<GameObject*> &go_using_res) const;

	virtual void Clear() { go_related.clear(); }

private:

	uint res_uid;
	std::vector<GameObject*> go_related;
	ResourceType res_type = ResourceType::None;
	char* archive = nullptr;
};

#endif
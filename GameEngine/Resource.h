#ifndef __Resource_H__
#define __Resource_H__

#include "Globals.h"
#include <vector>
#include <map>

class GameObject;

class Resource
{

public:
	enum ResourceType
	{
		Mesh,
		Transform,
		Material,
		None
	};

	Resource();

	Resource(ResourceType type, uint resource_uid, const char* resource_archive);
	ResourceType GetType() const;

	uint Count(std::vector<GameObject*> &go_using_res) const;

	void LoadBuffers();
	virtual void Clear() { gos_related.clear(); }

public:

	uint res_uid;
	std::vector<GameObject*> gos_related;
	ResourceType res_type = ResourceType::None;

	std::map<uint, std::string> uid_path;
	const char* archive = nullptr;
};

#endif
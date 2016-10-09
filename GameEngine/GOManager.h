#ifndef __GOManager_H__
#define __GOManager_H__

#include "Module.h"
#include "GameObject.h"

#include <vector>

class Application;

class GOManager : public Module
{
public:
	GOManager(Application* app, const char* name, bool start_enabled = true);
	~GOManager();

	bool Init(cJSON* node);
	//bool CleanUp();
	
	GameObject* CreateGo(const char* name, GameObject* parent) const;
	GameObject* FindGoParent(GameObject* child) const;

	//void LoadComponentInfo(GameObject* game_object, Component::ComponentType type) const;
	bool LoadFBXObjects(const char* FBX);
	
	std::vector<const char*> FBXs;

	GameObject* root_GO = nullptr;
	cJSON* GOs_node = nullptr;
};


#endif

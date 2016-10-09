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

	bool LoadFBXObjects(const char* FBX);
	

	update_status Update(float dt);
	void DrawGOs() const;


	bool load_fbx = true;
	GameObject* root_GO = nullptr;
};


#endif

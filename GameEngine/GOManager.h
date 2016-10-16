#ifndef __GOManager_H__
#define __GOManager_H__

#include "Module.h"
#include "GameObject.h"

#include <vector>

class Application;
class aiScene;
class aiNode;

class GOManager : public Module
{
public:
	GOManager(Application* app, const char* name, bool start_enabled = true);
	~GOManager();

	bool Init(cJSON* node);
	bool CleanUp();
	
	GameObject* CreateGo(const char* name, GameObject* parent) const;
	
	bool LoadFBXObjects(const char* FBX);
	bool LoadComponents(const aiScene* scene, const aiNode* node, GameObject* go) const;

	void EditorContent();
	void ShowToEditor(GameObject* go);

	update_status Update(float dt);

	void Draw() const;

private:

	bool load_fbx = true;

	GameObject* root_GO = nullptr;
	GameObject* selected = nullptr;

	bool enable = true;
	bool enable_mesh = true;
	bool enable_material = true;
};


#endif // __GOManager_H__
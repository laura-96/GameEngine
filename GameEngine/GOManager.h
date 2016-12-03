#ifndef __GOManager_H__
#define __GOManager_H__

#include "Module.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"

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
	
	GameObject* CreateGo(const char* name, uint uid, GameObject* parent) const;
	
	bool LoadFBXObjects(const char* FBX);
	bool LoadComponents(const char* prefab, GameObject* go) const;

	void CreateGOEditor(math::float2 editor_pos);
	void EditorContent();
	void ShowToEditor(GameObject* go);

	update_status Update(float dt);
	bool Save(cJSON* node);

	void Draw() const;
	void DrawSceneObjects() const;


	std::vector<GameObject*> root_objects;
	std::string loaded_fbx;
	bool save_tmp_file = false;

private:

	bool load_fbx = true;

	GameObject* root_GO = nullptr;
	GameObject* selected = nullptr;
	std::vector<GameObject*> created_objects;

	bool create_go_editor = false;
	math::float2 create_go_pos;

	//Used for editor content
	bool enable = true;
	bool enable_mesh = true;
	bool enable_material = true;
	bool enable_camera = true;

	//Used for camera content in editor
	bool camera_culling = true;
	float near_dist;
	float far_dist;
	float aspect_ratio;
	float fov_h;

	//Used for transform content in editor
	bool scale = false;
	bool rotate = false;
	bool translate = false;
	math::float3 _translation;
	math::float3 original_translation;
	math::float3 _scale;
	math::float3 original_scale;
	math::float3 euler;
	math::float3 original_euler;
	
};


#endif // __GOManager_H__
#ifndef __ModuleSceneImporter_H__
#define __ModuleSceneImporter_H__

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include <vector>

class Application;
class aiScene;
class aiNode;
class cJSON;

struct GO_reference {
	
	std::string name;
	uint num_children;
	std::vector<std::string> children_names;

};

class ModuleSceneImporter : public Module
{
public:
	ModuleSceneImporter(Application* app, const char* name, bool start_enabled = true);

	bool ImportScene(const char* path);
	bool Load(const char* exported_file, GameObject* go);
	bool Init(cJSON* node);
	bool Start();
	~ModuleSceneImporter();
	//bool ImportMesh(aiScene ai_scene, aiNode ai_node) const;
	//bool ImportMaterial(aiScene ai_scene, aiNode ai_node) const;

private:
	cJSON* importer;
	
};

#endif
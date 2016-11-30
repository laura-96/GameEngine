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


class ModuleSceneImporter : public Module
{
public:
	ModuleSceneImporter(Application* app, const char* name, bool start_enabled = true);

	bool Init(cJSON* node);
	bool Start();

	bool Save(cJSON* node);

	bool ImportScene(const char* path, std::string &output_scene);
	bool SaveScene(aiNode* node, const char* name, uint uid, uint p_uid, std::string &output, std::string prefab, cJSON* root) const;

	uint ImportPrefab(aiNode* node, uint mesh, uint material, std::string &output, const char* extension) const;
	uint ImportMesh(const aiScene* scene, aiNode* node, std::string &output, const char* extension) const;
	uint ImportMaterial(const aiScene* scene, aiNode* node, std::string &output) const;
	uint ImportMaterial(const char* directory, std::string &output) const;
	bool LoadTransform(aiNode* node, math::float3 &translation, math::Quat &rotation, math::float3 &scale) const;

	~ModuleSceneImporter();
	
};

#endif
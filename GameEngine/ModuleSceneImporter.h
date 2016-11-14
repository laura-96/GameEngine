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

struct Prefab {

public:
	Prefab(uint _uid, uint p_uid, std::string mesh, std::string tex) : uid(_uid), parent_uid(p_uid), mesh_related(mesh), texture_related(tex) {};
	
private:
	uint uid;
	uint parent_uid;

	std::string mesh_related;
	std::string texture_related;

public:
	math::float3 translation;
	math::Quat rotation;
	math::float3 scale;
};

class ModuleSceneImporter : public Module
{
public:
	ModuleSceneImporter(Application* app, const char* name, bool start_enabled = true);

	bool Init(cJSON* node);
	bool Start();

	bool Save(cJSON* node);

	bool ImportScene(const char* path);

	bool ImportPrefab(aiNode* node, uint mesh, uint material, std::string &output, const char* extension) const;
	uint ImportMesh(const aiScene* scene, aiNode* node, std::string &output, const char* extension) const;
	uint ImportMaterial(const aiScene* scene, aiNode* node, std::string &output) const;
	uint ImportMaterial(const char* path, std::string &output) const;
	bool LoadTransform(aiNode* node, math::float3 &translation, math::Quat &rotation, math::float3 &scale) const;

	bool LoadMesh(GameObject &go) const;
	bool LoadMaterial(GameObject &go) const;
	//The diffecence between both LoadTransform functions is that one, stores the information in an ObjectImport struct
	//and this following one, puts it into an object component
	bool LoadTransform(GameObject &go) const;
	~ModuleSceneImporter();
	
};

#endif
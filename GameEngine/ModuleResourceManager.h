#ifndef __ModuleResourceManager_H__
#define __ModuleResourceManager_H__

#include "Module.h"
#include <map>

class MaterialResource;
class MeshResource;

class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, const char* name, bool start_enabled = true);

	//bool Init(cJSON* node);
	bool Start();
	update_status Update(float dt);
	bool MonitorAssets();
	bool ImportFile(const char* file);
	uint ImportMesh(const char* file, uint uid);
	uint ImportMaterial(const char* file, uint uid);
	uint ImportMaterial(const char* file, const char* directory);
	void UpdateInfo(uint uid, uint image_id, uint texture[1]);

	bool IsMeshResource(const char* res) const;
	bool IsMaterialResource(const char* res) const;

	uint GetUidFromFile(const char* file) const;
	MeshResource* GetMeshResource(uint uid) const;
	MaterialResource* GetMaterialResource(uint uid) const;

	MaterialResource* CreateMaterialResource(uint uid, const char* path, uint id_image, uint texture[1]);
	MeshResource* CreateMeshResource(uint uid, const char* path);

	bool CleanUp();

private:
	Timer* monitor_assets = NULL;
	uint last_file_mod = NULL;

	//Equivalence between original file and the output
	std::map<std::string, std::string> res_equivalence;
	//Files and its modification
	std::map<std::string, uint> files_modifications;

	//Equivalence between resource path and uid
	std::map<std::string, uint> res_uid;

	//Equivalence between uid and its resource
	std::map<uint, MeshResource*> uid_mesh;
	std::map<uint, MaterialResource*> uid_material;
};

#endif
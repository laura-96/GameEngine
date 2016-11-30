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

	MeshResource* GetMeshResource(uint uid) const;
	MaterialResource* GetMaterialResource(uint uid) const;

	MaterialResource* CreateMaterialResource(uint uid, const char* path);
	MeshResource* CreateMeshResource(uint uid, const char* path);

private:
	Timer* monitor_assets = NULL;
	uint last_file_mod = NULL;
	std::map<std::string, std::string> res_equivalence;
	std::map<std::string, uint> files_modifications;

	std::map<uint, MeshResource*> uid_mesh;
	std::map<uint, MaterialResource*> uid_material;
};

#endif
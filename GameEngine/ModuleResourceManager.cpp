#include "Application.h"
#include "Globals.h"

#include "ModuleResourceManager.h"
#include "ModuleSceneImporter.h"

#include "ModuleFileSystem.h"

ModuleResourceManager::ModuleResourceManager(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{}

bool ModuleResourceManager::Start()
{
	monitor_assets = new Timer();
	last_file_mod = App->file_sys->GetLastModification("Game/Assets");
	return true;
}

update_status ModuleResourceManager::Update(float dt)
{

	if (monitor_assets->Read() > 1000) //Time in ms
	{
		monitor_assets->Stop();
		monitor_assets->Start();
		MonitorAssets();
	}

	return UPDATE_CONTINUE;
}

bool ModuleResourceManager::MonitorAssets() const
{

	std::vector<const char*> files;
	App->file_sys->CollectFiles("Assets", files);

	uint last_mod = App->file_sys->GetLastModification("Game/Assets");

	if (last_mod != last_file_mod)
	{
		LOG("Something changed in Assets directory");
	}

	for (uint i = 0; i < files.size(); i++)
	{
		ImportFile(files[i]);
	}

	return true;
}

bool ModuleResourceManager::ImportFile(const char* file) const
{
	std::string file_type;
	App->file_sys->GetExtension(file, file_type);

	if(strcmp(file_type.c_str(), "ogg") == 0)
	{
		LOG("File: %s is an audio", file);
	}

	if (strcmp(file_type.c_str(), "png") == 0)
	{
		LOG("File: %s is an image", file);
	}

	if (strcmp(file_type.c_str(), "tga") == 0)
	{
		LOG("File: %s is an image", file);
	}

	if (strcmp(file_type.c_str(), "fbx") == 0 || strcmp(file_type.c_str(), "FBX") == 0)
	{
		LOG("File: %s is a scene", file);
	}

	else
	{
		LOG("Warning, file type not allowed");
	}

	return true;
}


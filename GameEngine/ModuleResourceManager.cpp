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

	std::vector<std::string> files;

	App->file_sys->CollectFiles("Assets", files);

	for (uint i = 0; i < files.size(); i++)
	{
		ImportFile(files[i].c_str());
	}

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

bool ModuleResourceManager::MonitorAssets()
{

	uint last_mod = App->file_sys->GetLastModification("Game/Assets");

	if (last_mod != last_file_mod)
	{
		LOG("Something changed in Assets directory");
	}

	return true;
}

bool ModuleResourceManager::ImportFile(const char* directory)
{
	std::string file_type;
	std::string file;

	App->file_sys->GetFileFromDir(directory, file);
	App->file_sys->GetExtension(file.c_str(), file_type);


	if(strcmp(file_type.c_str(), "ogg") == 0)
	{
		LOG("File: %s is an audio", file.c_str());
	}

	if (strcmp(file_type.c_str(), "tga") == 0 || strcmp(file_type.c_str(), "png") == 0)
	{
		std::string output;
		App->scene_importer->ImportMaterial(directory, output);
		
		res_equivalence.insert(std::pair<std::string, std::string>(file, output));

		LOG("File: %s is an image", file.c_str());
	}

	if (strcmp(file_type.c_str(), "fbx") == 0 || strcmp(file_type.c_str(), "FBX") == 0)
	{
		LOG("File: %s is a scene", file.c_str());
	}

	else
	{
		LOG("Warning, file type not allowed");
	}

	return true;
}


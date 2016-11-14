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

bool ModuleResourceManager::MonitorAssets()
{
	uint last_mod = App->file_sys->GetLastModification("Game/Assets");

	if (last_mod != last_file_mod)
	{
		LOG("Something changed in Assets directory");
	}
	return true;
}


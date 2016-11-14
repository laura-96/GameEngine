#ifndef __ModuleResourceManager_H__
#define __ModuleResourceManager_H__

#include "Module.h"

class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, const char* name, bool start_enabled = true);

	//bool Init(cJSON* node);
	bool Start();
	update_status Update(float dt);
	bool MonitorAssets();

private:
	Timer* monitor_assets = NULL;
	uint last_file_mod = NULL;
};

#endif
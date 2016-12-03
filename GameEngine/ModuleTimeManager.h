#ifndef __ModuleTimeManager_H__
#define __ModuleTimeManager_H__

#include "Module.h"
#include "Globals.h"
#include "Imgui/imgui.h"

class ModuleTimeManager : public Module
{
public:

	ModuleTimeManager(Application* app, const char* name, bool start_enabled = true);
	~ModuleTimeManager() {};

	void ManagementTimeEditor();

	bool Init(cJSON* node) { return true; };
	update_status Update(float dt);
	bool CleanUp() { return true; };

private:
	float dt = 0;

};


#endif

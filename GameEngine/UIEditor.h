#ifndef __UIEditor_H__
#define __UIEditor_H__

#include "Module.h"
#include <vector>

class Panel;
class PanelConfiguration;
class PanelConsole;

class UIEditor : public Module
{

public:
	UIEditor(Application* app, const char* name, bool start_enabled = true);
	~UIEditor();

	bool Init(cJSON* node);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool Draw();

	bool CleanUp();

	bool capture_mouse = false;
	bool capture_keyboard = false;

	std::vector<Panel*> panels;

	PanelConfiguration* conf = nullptr;
	PanelConsole* console = nullptr;
};

#endif // __UIEditor_H__
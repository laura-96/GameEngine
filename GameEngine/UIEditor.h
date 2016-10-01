#pragma once

#include "Module.h"

class UIEditor : public Module
{

public:
	UIEditor(Application* app, bool start_enabled = true);
	~UIEditor();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool Draw();

	bool CleanUp();

	bool capture_mouse = false;
	bool capture_keyboard = false;
};
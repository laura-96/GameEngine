#pragma once

#include "Module.h"

class GUI : public Module
{

public:
	GUI(Application* app, bool start_enabled = true);
	~GUI();

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
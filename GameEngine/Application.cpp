#include "Application.h"

#include "Module.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "UIEditor.h"

Application::Application()
{
	file_sys = new ModuleFileSystem(this);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	physics = new ModulePhysics3D(this);
	imgui = new UIEditor(this);
	
	//player = new ModulePlayer(this); 

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(file_sys);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);

	// Scenes
	AddModule(scene_intro);

	// Renderer
	AddModule(renderer3D);

	// GUI
	AddModule(imgui);

}

Application::~Application()
{
	list<Module*>::reverse_iterator it = list_modules.rbegin();

	while (it != list_modules.rend())
	{
		delete (*it);
		it++;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	list<Module*>::iterator it = list_modules.begin();

	while (it != list_modules.end() && ret == true)
	{
		ret = (*it)->Init();
		it++;
	}

	// After all Init calls we call Start() in all modules
	LOG("__Application Start__");

	it = list_modules.begin();
	

	while (it != list_modules.end() && ret == true)
	{
		ret = (*it)->Start();
		it++;
	}

	ms_timer.Start();
	return ret;
}

void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	PrepareUpdate();

	list<Module*>::iterator it = list_modules.begin();

	while (it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*it)->PreUpdate(dt);
		it++;
	}

	it = list_modules.begin();

	while (it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*it)->Update(dt);
		it++;
	}

	it = list_modules.begin();

	while (it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*it)->PostUpdate(dt);
		it++;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	list<Module*>::reverse_iterator it = list_modules.rbegin();

	while (it != list_modules.rend() && ret == true)
	{
		ret = (*it)->CleanUp();
		it++;
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}
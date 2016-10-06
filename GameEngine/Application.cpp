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
#include "cJSON.h"

using namespace std;

Application::Application()
{
	file_sys = new ModuleFileSystem(this, "File_System");
	window = new ModuleWindow(this, "Window");
	input = new ModuleInput(this, "Input");
	audio = new ModuleAudio(this, "Audio", true);
	scene_intro = new ModuleSceneIntro(this, "Scene_Intro");
	renderer3D = new ModuleRenderer3D(this, "Renderer");
	camera = new ModuleCamera3D(this, "Camera");
	physics = new ModulePhysics3D(this, "Physics");
	imgui = new UIEditor(this, "UI_Editor");
	
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

	char* buff = nullptr;
	file_sys->Load("Config.json", &buff);

	if (buff != nullptr)
	{
		root = cJSON_Parse(buff);

		cJSON * App = cJSON_GetObjectItem(root, "App");
		title = cJSON_GetObjectItem(App, "Title")->valuestring;
		organization = App->child->next->valuestring;
		
	}


	// Call Init() in all modules
	list<Module*>::iterator it = list_modules.begin();

	while (it != list_modules.end() && ret == true)
	{
		ret = (*it)->Init(cJSON_GetObjectItem(root, (*it)->GetName()));
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

	if (root != nullptr)
	{
		cJSON_Delete(root);
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

const char* Application::GetTitle() const
{
	return title.c_str();
}

const char* Application::GetOrganization() const
{
	return organization.c_str();
}
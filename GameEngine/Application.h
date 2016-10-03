#pragma once

#include <list>
#include "Globals.h"
#include "Timer.h"

class Module;
class ModuleFileSystem;
class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleSceneIntro;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModulePhysics3D;
class UIEditor;

using namespace std;

class Application
{
public:
	
	ModuleFileSystem* file_sys;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	UIEditor* imgui;
	//ModulePlayer* player;

private:
	string	title;
	string	organization;
	Timer	ms_timer;
	float	dt;
	list<Module*> list_modules;

public:
	const char* GetTitle() const;
	const char* GetOrganization() const;


public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();


};
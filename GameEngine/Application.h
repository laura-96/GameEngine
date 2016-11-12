#ifndef __Application_H__
#define __Application_H__

#include <list>
#include "Globals.h"
#include "Timer.h"

class Module;
class ModuleFileSystem;
class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class GOManager;
class ModuleSceneImporter;
class ModuleSceneIntro;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModulePhysics3D;
class UIEditor;
class cJSON;


class Application
{
public:
	
	ModuleFileSystem* file_sys;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	GOManager* go_manager;
	ModuleSceneImporter* scene_importer;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	UIEditor* imgui;
	//ModulePlayer* player;

private:
	cJSON* root;
	std::string	title;
	std::string	organization;
	Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;

public:
	const char* GetTitle() const;
	const char* GetOrganization() const;


public:

	Application();
	~Application();

	bool Init();
	bool Save();
	update_status Update();
	bool CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();


};

#endif // __Application_H__
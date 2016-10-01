#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	m = new Mesh();
	m->LoadMesh("Game/Assets/warrior.FBX");
	//b = new InCube();
	bool ret = true;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	m->CleanUp();
	//delete b;
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{

	return UPDATE_CONTINUE;
}

bool ModuleSceneIntro::Draw()
{
	//b->Render();
	
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	m->Render();

	return true;
}
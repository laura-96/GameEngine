#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	//LOG("Loading Intro assets");
	//m = new Mesh();
	
	//m->LoadMesh("Game/Assets/Brute.fbx");
	//b = new InCube("Game/Assets/Textures/Lenna.png");
	bool ret = true;

	return ret;
}

bool ModuleSceneIntro::Init(cJSON* node)
{
	 
	return true;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	//LOG("Unloading Intro scene");
	//m->CleanUp();
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
	//b->RenderText();
	PPlane p(0, 0, 0, 0);
	p.axis = true;
	p.Render();

	//m->Render();
	
	return true;
}
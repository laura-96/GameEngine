#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Physfs/src/physfs.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"

#pragma comment( lib, "Physfs/libx86/physfs.lib" )

using namespace std;

ModuleFileSystem::ModuleFileSystem(Application* app, const char* game_path) : Module(app, true)
{
	// need to be created before Awake so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

}

bool ModuleFileSystem::Init()
{
	LOG("Loading File System");
	bool ret = true;


	return ret;
}

ModuleFileSystem::~ModuleFileSystem()
{}

bool ModuleFileSystem::CleanUp()
{

	return true;
}

int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}
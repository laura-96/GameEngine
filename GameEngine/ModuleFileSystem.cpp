#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Physfs/src/physfs.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"

#pragma comment( lib, "Physfs/libx86/physfs.lib" )

using namespace std;

ModuleFileSystem::ModuleFileSystem(Application* app, const char* name, const char* game_path) : Module(app, name, true)
{
	// need to be created before Awake so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	

	if (PHYSFS_setWriteDir(base_path) == NULL)
	{
		LOG("Error while setting write directory: %s", PHYSFS_getLastError());
	}

	else
	{
		AddPath(".");

		if (game_path != nullptr)
		{
			PHYSFS_mkdir(game_path);
		}

		if (!IsDirectory("Assets"))
		{
			PHYSFS_mkdir("Game/Assets");
		}

		if (!IsDirectory("Library"))
		{
			PHYSFS_mkdir("Game/Library");
		}
	}

	SDL_free(base_path);
}

bool ModuleFileSystem::Init(cJSON* node)
{
	LOG("Loading File System");
	bool ret = true;

	// Add all paths in configuration in order
	/*for (pugi::xml_node path = config.child("path"); path; path = path.next_sibling("path"))
	{
		AddPath(path.child_value());
	}*/

	// Ask SDL for a write dir
	char* write_path = SDL_GetPrefPath(App->GetOrganization(), App->GetTitle());
	
	if (PHYSFS_setWriteDir(write_path) == 0)
	{
		LOG("File System error while creating write dir: %s", PHYSFS_getLastError());
	}
		
	else
	{
		// We add the writing directory as a reading directory too with speacial mount point
		LOG("Writing directory is %s", write_path);
		//AddPath(write_path, GetSaveDirectory());
	}

	SDL_free(write_path);
	
	return ret;
}

bool ModuleFileSystem::AddPath(const char* path_or_zip, const char* mount_point)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, mount_point, 1) == 0)
	{
		LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}

	else
		ret = true;

	return ret;
}

bool ModuleFileSystem::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

bool ModuleFileSystem::IsDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}

unsigned int ModuleFileSystem::Load(const char* file, char** buffer) const
{
	unsigned int ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != NULL)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);

		if (size > 0)
		{
			*buffer = new char[(uint)size];
			PHYSFS_sint64 readed = PHYSFS_read(fs_file, *buffer, 1, (PHYSFS_sint32)size);
			if (readed != size)
			{
				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE(buffer);
			}
			else
				ret = (uint)readed;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

SDL_RWops* ModuleFileSystem::Load(const char* file) const
{
	char* buffer;
	int size = Load(file, &buffer);

	if (size > 0)
	{
		SDL_RWops* r = SDL_RWFromConstMem(buffer, size);
		if (r != NULL)
			r->close = close_sdl_rwops;

		return r;
	}
	else
		return NULL;
}

unsigned int ModuleFileSystem::Save(const char* file, const char* buffer, unsigned int size) const
{
	unsigned int ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openWrite(file);

	if (fs_file != NULL)
	{
		PHYSFS_sint64 written = PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG("File System error while writing to file %s: %s\n", file, PHYSFS_getLastError());
		}		
		else
		{
			ret = (uint)written;
		}

		if (PHYSFS_close(fs_file) == 0)
		{
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
		}
	}
	else
	{
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());
	}

	return ret;
}

ModuleFileSystem::~ModuleFileSystem()
{}

bool ModuleFileSystem::CleanUp()
{
	//RELEASE(AssimpIO);
	
	return true;
}

int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}
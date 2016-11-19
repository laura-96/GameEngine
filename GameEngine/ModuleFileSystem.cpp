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
			PHYSFS_addToSearchPath(game_path, NULL);
		}

		if (!IsDirectory("Assets"))
		{
			PHYSFS_mkdir("Game/Assets");
			PHYSFS_addToSearchPath("Game/Assets", NULL);
		}

		if (!IsDirectory("Library"))
		{
			PHYSFS_mkdir("Game/Library");
			PHYSFS_addToSearchPath("Game/Library", NULL);
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

void ModuleFileSystem::GetFilesModified(const char* file, std::map<std::string, uint> &mod_files) const
{
	std::vector<string> files;
	CollectFiles(file, files);

	std::vector<string>::iterator it = files.begin();

	while (it != files.end())
	{
		mod_files.insert(pair<string, uint>((*it), GetLastModification((*it).c_str())));
		it++;
	}

}

uint ModuleFileSystem::GetLastModification(const char* file) const
{
	return PHYSFS_getLastModTime(file);
}

void ModuleFileSystem::CollectFiles(const char* directory, std::vector<std::string> &files) const
{

	char** dir_files = PHYSFS_enumerateFiles(directory);

	if (dir_files[0] != NULL)
	{
		for (uint i = 0; dir_files[i] != NULL; i++)
		{
			//If files or directories are not added in the search path, PHYSFS_enumerateFiles won't find any files that these ones could contain if they are directories
			std::string dir;
			dir.clear();
			dir.append(directory);
			dir.append("/");
			dir.append(dir_files[i]);

			PHYSFS_addToSearchPath(dir.c_str(), NULL);

			if (IsDirectory(dir.c_str()))
			{
				CollectFiles(dir.c_str(), files);
			}

			else
			{
				//files.push_back(dir_files[i]);

				files.push_back(dir);
			}

		}
	}
}

void ModuleFileSystem::GetExtension(const char* file, std::string &ext) const
{

	std::string archive(file);
	size_t index = archive.find_last_of(".");

	if (index < archive.length())
	{
		ext = archive.substr(index + 1);
	}

}

uint ModuleFileSystem::GetFileFromDir(const char* directory, std::string &file) const
{
	std::string dir_file;
	dir_file.append(directory);
	size_t index = dir_file.find_last_of("/");

	file = dir_file.substr(index + 1);

	return file.size();
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

unsigned int ModuleFileSystem::SaveInDir(const char* directory, const char* file, const void* buffer, unsigned int size)
{
	uint ret = NULL;

	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);

	if (PHYSFS_setWriteDir(base_path) == NULL)
	{
		LOG("Error while setting write directory: %s", PHYSFS_getLastError());
	}

	else
	{
		AddPath(".");
	}


	if (!IsDirectory(directory))
	{
		PHYSFS_mkdir(directory);
	}

	std::string write_dir;
	write_dir.append(base_path);
	write_dir.append("/");
	write_dir.append(directory);

	if (PHYSFS_setWriteDir(write_dir.c_str()) == NULL)
	{
		LOG("Error while setting write directory: %s", PHYSFS_getLastError());
	}

	else
	{
		AddPath(".");
	}

	SDL_free(base_path);
	SDL_free((void*)write_dir.c_str());

	ret = Save(file, buffer, size);

	const char* save_path = SDL_GetPrefPath(App->GetOrganization(), App->GetTitle());

	if (PHYSFS_setWriteDir(save_path) == 0)
	{
		LOG("File System error while creating write dir: %s", PHYSFS_getLastError());
	}

	else
	{
		// We add the writing directory as a reading directory too with speacial mount point
		LOG("Writing directory is %s", save_path);
	}

	SDL_free((void*)save_path);

	return ret;
}

unsigned int ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size) const
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
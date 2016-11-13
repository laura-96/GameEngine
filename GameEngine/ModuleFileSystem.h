#ifndef __ModuleFileSystem_H__
#define __ModuleFileSystem_H__

#include "Module.h"
#include "Globals.h"
#include "Physfs/src/physfs.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"

#include "cJSON.h"
#include <vector>


struct SDL_RWops;
struct aiFileIO;

int close_sdl_rwops(SDL_RWops *rw);


class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, const char* name, const char* game_path = nullptr);

	// Destructor
	~ModuleFileSystem();
	
	bool Init(cJSON* node);
	bool CleanUp();
	
	bool AddPath(const char* path_or_zip, const char* mount_point = nullptr);
	bool Exists(const char* file) const;
	bool IsDirectory(const char* file) const;
	const char* GetWriteDirectory() const
	{
		return PHYSFS_getWriteDir();
	}
	
	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	unsigned int SaveInDir(const char* directory, const char* file, const void* buffer, unsigned int size);
	unsigned int Save(const char* file, const void* buffer, unsigned int size) const;
	//aiFileIO* GetAiFileIO() const;

private:

	aiFileIO* ai_file = nullptr;
};

#endif // __ModuleFileSystem_H__
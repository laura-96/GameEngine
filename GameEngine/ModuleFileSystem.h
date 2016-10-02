#ifndef __MODULE_FILESYSTEM_H__
#define __MODULE_FILESYSTEM_H__

#include "Module.h"
#include "Globals.h"
#include "Physfs/src/physfs.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"

#include "cJSON.h"
#include <vector>




struct SDL_RWops;

int close_sdl_rwops(SDL_RWops *rw);

struct aiFileIO;

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, const char* game_path = nullptr);

	// Destructor
	~ModuleFileSystem();
	
	bool Init();
	bool CleanUp();
	/*
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	bool IsDirectory(const char* file) const;
	bool CopyFromOutsideFS(const char* full_path, const char* destination);

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	aiFileIO* GetAssimpIO();

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);

	const char* GetBasePath() const;
	const char* GetWritePath() const;
	const char* GetReadPaths() const;

private:

	void CreateAssimpIO();

private:
*/
	aiFileIO* AssimpIO = nullptr;
	
};

#endif
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
	uint GetLastModification(const char* file) const;
	const char* GetWriteDirectory() const
	{
		return PHYSFS_getWriteDir();
	}

	//Utility function to collect in a vector all files contained in a directory and its subdirectories
	void CollectFiles(const char* directory, std::vector<std::string> &files) const;
	//Utility to get extension of a file
	void GetExtension(const char* file, std::string &ext) const;
	uint GetFileFromDir(const char* directory, std::string &file) const;

	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	unsigned int SaveInDir(const char* directory, const char* file, const void* buffer, unsigned int size);
	unsigned int Save(const char* file, const void* buffer, unsigned int size) const;
	//aiFileIO* GetAiFileIO() const;

private:

	aiFileIO* ai_file = nullptr;
};

#endif // __ModuleFileSystem_H__
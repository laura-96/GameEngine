#include "Application.h"
#include "Globals.h"

#include "ModuleResourceManager.h"
#include "ModuleSceneImporter.h"

#include "Resource.h"
#include "MaterialResource.h"
#include "MeshResource.h"

#include "ModuleFileSystem.h"

ModuleResourceManager::ModuleResourceManager(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{}

bool ModuleResourceManager::Start()
{
	monitor_assets = new Timer();
	last_file_mod = App->file_sys->GetLastModification("Game/Assets");

	std::vector<std::string> files;

	App->file_sys->CollectFiles("Assets", files);
	App->file_sys->GetFilesModified("Assets", files_modifications);

	for (uint i = 0; i < files.size(); i++)
	{
		ImportFile(files[i].c_str());
	}

	return true;
}

update_status ModuleResourceManager::Update(float dt)
{

	if (monitor_assets->Read() > 1000) //Time in ms
	{
		monitor_assets->Stop();
		monitor_assets->Start();
		MonitorAssets();
	}

	return UPDATE_CONTINUE;
}

bool ModuleResourceManager::MonitorAssets()
{
	bool modified = false;

	//Collecting all files found in assets
	std::vector<std::string> files;
	App->file_sys->CollectFiles("Assets", files);

	//Get modifications of each file
	std::map<std::string, uint> modifications;
	App->file_sys->GetFilesModified("Assets", modifications);

	//Comparing current modifications to last ones
	for (uint i = 0; i < files.size(); i++)
	{
		std::map<std::string, uint>::iterator it_last_mod = files_modifications.find(files[i]);
		std::map<std::string, uint>::iterator it_curr_mod = modifications.find(files[i]);

		//Comparing the iterator to end() means checking if it is null
		//find() function returns end() if the specified key is not found in the map
		if (it_last_mod == files_modifications.end())
		{
			if (!modified)
				modified = true;
	
			ImportFile(files[i].c_str());
			LOG("New file was added or an existing file was renamed");
		}

		else
		{
			if ((*it_last_mod).second != (*it_curr_mod).second)
			{
				if (!modified)
					modified = true;
	
				LOG("%s has been modified", files[i].c_str());
			}
		}	
	}

	//Checking if something has been deleted
	std::map<std::string, uint>::iterator it_last_mod = files_modifications.begin();

	while (it_last_mod != files_modifications.end())
	{
		
		std::map<std::string, uint>::iterator it_curr_mod = modifications.find(it_last_mod->first.c_str());

		if (it_curr_mod == modifications.end())
		{
			if (!modified)
				modified = true;

			LOG("%s has been deleted", it_last_mod->first.c_str());
		}

		it_last_mod++;
	}


	return modified;
}

bool ModuleResourceManager::ImportFile(const char* directory)
{
	bool ret = true;

	std::string file_type;
	std::string file;

	App->file_sys->GetFileFromDir(directory, file);
	App->file_sys->GetExtension(file.c_str(), file_type);

	//Depending on the extension of the file, one import function is called or another
	if (strcmp(file_type.c_str(), "ogg") == 0)
	{
		LOG("File: %s is an audio", file.c_str());
	}

	else if (strcmp(file_type.c_str(), "tga") == 0 || strcmp(file_type.c_str(), "png") == 0)
	{
		std::string output;
		uint uid = App->scene_importer->ImportMaterial(directory, output);

		//Creating equivalence to be used to look for original file from the imported one
		res_equivalence.insert(std::pair<std::string, std::string>(file, output));

		std::string dir;
		dir.clear();
		dir.append("Game/Library/Material/");
		dir.append(output);

		CreateMaterialResource(uid, dir.c_str());

		LOG("File: %s is an image", file.c_str());
	}

	else if (strcmp(file_type.c_str(), "fbx") == 0 || strcmp(file_type.c_str(), "FBX") == 0)
	{
		std::string output;
		App->scene_importer->ImportScene(directory, output);
		
		LOG("File: %s is a scene", file.c_str());
	}

	else
	{
		ret = false;
		LOG("Warning, file type not allowed");
	}

	return ret;
}

MaterialResource* ModuleResourceManager::CreateMaterialResource(uint uid, const char* path) const
{
	MaterialResource* material = new MaterialResource(uid, path);

	material->SetPath(path);

	return material;
}

MeshResource* ModuleResourceManager::CreateMeshResource(uint uid, const char* path) const
{
	MeshResource* mesh = new MeshResource(uid, path);

	return mesh;
}

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

				std::string file_type;
				std::string file;

				App->file_sys->GetFileFromDir(files[i].c_str(), file);
				App->file_sys->GetExtension(file.c_str(), file_type);

				if (strcmp(file_type.c_str(), "tga") == 0 || strcmp(file_type.c_str(), "png") == 0)
				{
					App->scene_importer->ReimportMaterial(files[i].c_str());
				}
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
		ImportMaterial(file.c_str(), directory);
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

uint ModuleResourceManager::GetUidFromFile(const char* file) const
{
	uint ret = NULL;
	std::map<std::string, uint>::const_iterator it = res_uid.find(file);
	
	if (it != res_uid.end())
	{
		ret = it->second;
	}
	
	return ret;
}

uint ModuleResourceManager::ImportMaterial(const char* file, const char* directory)
{
	uint uid = NULL;

	std::string output;
	std::map<std::string, uint>::iterator it = res_uid.find(file);

	if (it == res_uid.end())
	{
		uid = App->scene_importer->ImportMaterial(directory, output);
	}

	else
	{
		uid = res_uid.find(file)->second;
	}

	return uid;
}

uint ModuleResourceManager::ImportMesh(const char* file, uint uid)
{
	res_uid.insert(std::pair<std::string, uint>(file, uid));

	return uid;
}

uint ModuleResourceManager::ImportMaterial(const char* file, uint uid)
{
	res_uid.insert(std::pair<std::string, uint>(file, uid));

	return uid;
}

MaterialResource* ModuleResourceManager::GetMaterialResource(uint uid) const
{
	MaterialResource* ret = nullptr;

	std::map<uint, MaterialResource*>::const_iterator it = uid_material.find(uid);
	if (it != uid_material.end())
	{
		ret = (*it).second;
	}

	return ret;
}

bool ModuleResourceManager::IsMeshResource(const char* res) const
{
	bool ret = false;

	std::map<std::string, uint>::const_iterator it = res_uid.find(res);
	
	if (it != res_uid.end())
	{
		ret = true;
	}

	return ret;
}
bool ModuleResourceManager::IsMaterialResource(const char* res) const
{
	bool ret = false;

	std::map<std::string, uint>::const_iterator it = res_uid.find(res);

	if (it != res_uid.end())
	{
		ret = true;
	}

	return ret;
}

MeshResource* ModuleResourceManager::GetMeshResource(uint uid) const
{
	MeshResource* ret = nullptr;

	std::map<uint, MeshResource*>::const_iterator it = uid_mesh.find(uid);
	if (it != uid_mesh.end())
	{
		ret = (*it).second;
	}

	return ret;
}

MaterialResource* ModuleResourceManager::CreateMaterialResource(uint uid, const char* path, uint id_image, uint texture[1])
{
	MaterialResource* material = new MaterialResource(uid, path);
	uid_material.insert(std::pair<uint, MaterialResource*>(uid, material));
	
	material->id_image = id_image;
	material->texture[0] = texture[0];
	material->SetPath(path);

	return material;
}

MeshResource* ModuleResourceManager::CreateMeshResource(uint uid, const char* path)
{
	MeshResource* mesh = new MeshResource(uid, path);
	
	char* buffer = nullptr;
	std::string dir;
	dir.append("Game/Library/Mesh/");
	dir.append(path);

	App->file_sys->Load(dir.c_str(), &buffer);

	if (buffer != nullptr)
	{
		char* cursor = buffer;

		uint attributes[4];

		memcpy(attributes, cursor, sizeof(attributes));
		mesh->num_index = attributes[0];
		mesh->num_vertex = attributes[1];
		mesh->num_normals = attributes[2];
		mesh->num_uvs = attributes[3];

		cursor += sizeof(attributes);

		mesh->indices = new uint[attributes[0]];
		memcpy(mesh->indices, cursor, sizeof(uint) * attributes[0]);

		cursor += (sizeof(uint) * attributes[0]);

		mesh->vertices = new float[attributes[1] * 3];
		memcpy(mesh->vertices, cursor, sizeof(float) * attributes[1] * 3);

		cursor += (sizeof(float) * attributes[1] * 3);

		if (attributes[2] > 0)
		{
			mesh->normals = new float[attributes[2] * 3];
			memcpy(mesh->normals, cursor, sizeof(float) * attributes[2] * 3);

			cursor += sizeof(float) * attributes[2] * 3;
		}

		if (attributes[3] > 0)
		{
			mesh->uvs = new float[attributes[3] * 2];
			memcpy(mesh->uvs, cursor, sizeof(float) * attributes[2] * 2);

			cursor += sizeof(float) * attributes[3] * 2;
		}
	}

	uid_mesh.insert(std::pair<uint, MeshResource*>(uid, mesh));

	return mesh;
}

void ModuleResourceManager::UpdateInfo(uint uid, uint image_id, uint texture[1])
{
	MaterialResource* material = nullptr;

	std::map<uint, MaterialResource*>::iterator it = uid_material.find(uid);

	if (it != uid_material.end())
	{
		material = it->second;
	}

	if (material)
	{
		material->id_image = image_id;
		material->texture[0] = texture[0];
	}

}

bool ModuleResourceManager::CleanUp()
{
	
	res_equivalence.clear();
	files_modifications.clear();

	res_uid.clear();

	std::map<uint, MeshResource*>::iterator it_mesh = uid_mesh.begin();

	while (it_mesh != uid_mesh.end())
	{
		std::vector<GameObject*> using_res;
		
		uint obj_count = it_mesh->second->Count(using_res);

		if (obj_count == 0)
		{
			it_mesh->second->Clear();
		}

		using_res.clear();

		it_mesh++;
	}

	uid_mesh.clear();


	std::map<uint, MaterialResource*>::iterator it_mat = uid_material.begin();

	while (it_mat != uid_material.end())
	{
		std::vector<GameObject*> using_res;

		uint obj_count = it_mat->second->Count(using_res);

		if (obj_count == 0)
		{
			it_mat->second->Clear();
		}

		using_res.clear();

		it_mat++;
	}

	uid_material.clear();

	return true;
}

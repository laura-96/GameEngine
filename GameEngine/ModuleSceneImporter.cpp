#include "Application.h"
#include "Globals.h"

#include "ModuleSceneImporter.h"

#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "cJSON.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "Devil/include/il.h"
#include "Devil/include/ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )


ModuleSceneImporter::ModuleSceneImporter(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{}

ModuleSceneImporter::~ModuleSceneImporter()
{}

bool ModuleSceneImporter::Init(cJSON* node)
{
	ilInit();
	ilutInit();

	ilutRenderer(ILUT_OPENGL);

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleSceneImporter::Start()
{
	//App->scene_importer->ImportScene("Game/Assets/Town/Street.FBX");

	return  true;
}

bool ModuleSceneImporter::Save(cJSON* node)
{
	

	return true;
}

bool ModuleSceneImporter::ImportScene(const char* file, std::string &output_scene)
{
	bool ret = true;

	std::string _file;
	_file.clear();
	_file.append("Game/");
	_file.append(file);

	const aiScene* scene = aiImportFile(_file.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->mRootNode != nullptr && scene->HasMeshes())
	{
		aiNode* node = scene->mRootNode;
		std::vector<aiNode*> closed_nodes;

		math::LCG random = math::LCG();

		char* buffer = "{}";
		cJSON* root = cJSON_Parse(buffer);

		std::string file_name;
		std::string output_json;

		std::string output;
		std::string output_image;
		std::string output_prefab;

		uint rmesh_uid = ImportMesh(scene, node, output, "lau");

		uint rmat_uid = ImportMaterial(scene, node, output_image);
		uint prefab_uid = ImportPrefab(node, rmesh_uid, rmat_uid, output_prefab, "gl");


		App->file_sys->GetFileFromDir(file, file_name);
		
		uint parent_uid = random.IntFast();
		SaveScene(node, file_name.c_str(), parent_uid, NULL, output_json, output_prefab, root);

		while (node != nullptr)
		{
			if (node->mNumChildren > 0)
			{
				uint i = 0;

				for (uint j = 0; j < closed_nodes.size(); j++)
				{
					if (closed_nodes[j] == node->mChildren[i])
					{
						i++;
					}
				}

				if (i >= node->mNumChildren)
				{
					closed_nodes.push_back(node);
				}
				
				else
				{
					node = node->mChildren[i];

					std::string output;
					std::string output_image;
					std::string output_prefab;

					uint im_uid = ImportMesh(scene, node, output, "lau");
					
					uint mat_uid = ImportMaterial(scene, node, output_image);
					
					prefab_uid = ImportPrefab(node, im_uid, mat_uid, output_prefab, "gl");

					uint go_uid = random.IntFast();

					SaveScene(node, file_name.c_str(), go_uid, parent_uid, output_json, output_prefab, root);

					parent_uid = go_uid;

					if (node->mParent != nullptr)
					{
						LOG("Parent: %s ---- Node name: %s", node->mParent->mName.C_Str(), node->mName.C_Str());
					}
					else
					{
						LOG("Node name: %s", node->mName.C_Str());
					}
				}
			}

			else
			{
				closed_nodes.push_back(node);
			}

			for (uint j = 0; j < closed_nodes.size(); j++)
			{
				if (closed_nodes[j] == node)
				{

					if (node)
					{
						cJSON* parent = cJSON_GetObjectItem(root, node->mName.C_Str());
						parent_uid = cJSON_GetObjectItem(parent, "Parent UID")->valueint;
					}

					node = node->mParent;
				}
			}
		}
		closed_nodes.clear();

		output_scene.clear();
		output_scene.append(output_json);
	}

	else
	{
		ret = false;
		LOG("Error loading scene: %s", file);
	}

	aiReleaseImport(scene);

	return ret;
}

bool ModuleSceneImporter::SaveScene(aiNode* node, const char* name, uint uid, uint p_uid, std::string &output, std::string prefab, cJSON* root) const
{
	bool ret = false;
	
	char* save = cJSON_Print(root);
	uint size = 0;

	cJSON* go = cJSON_CreateObject();
	cJSON* transl = cJSON_CreateObject();
	cJSON* rotate = cJSON_CreateObject();
	cJSON* scl = cJSON_CreateObject();

	math::float3 translation;
	math::Quat rotation;
	math::float3 scale;

	if (save)
	{
		size = strlen(save);
	}

	cJSON_AddItemToObject(root, node->mName.C_Str(), go);
	
	cJSON_AddNumberToObject(go, "UID", uid);
	cJSON_AddNumberToObject(go, "Parent UID", p_uid);

	if (prefab.size() > 0)
	{
		cJSON_AddStringToObject(go, "Prefab", prefab.c_str());
	}
	
	LoadTransform(node, translation, rotation, scale);


	cJSON_AddItemToObject(go, "Translation", transl);

	cJSON_AddNumberToObject(transl, "x", translation.x);
	cJSON_AddNumberToObject(transl, "y", translation.y);
	cJSON_AddNumberToObject(transl, "z", translation.z);

	cJSON_AddItemToObject(go, "Rotation", rotate);

	cJSON_AddNumberToObject(rotate, "x", rotation.x);
	cJSON_AddNumberToObject(rotate, "y", rotation.y);
	cJSON_AddNumberToObject(rotate, "z", rotation.z);
	cJSON_AddNumberToObject(rotate, "w", rotation.w);

	cJSON_AddItemToObject(go, "Scale", scl);

	cJSON_AddNumberToObject(scl, "x", scale.x);
	cJSON_AddNumberToObject(scl, "y", scale.y);
	cJSON_AddNumberToObject(scl, "z", scale.z);


	save = cJSON_Print(root);
	size = strlen(save);

	cJSON_Parse(save);

	output.clear();
	output.append(name);
	output.append(".json");

	App->file_sys->SaveInDir("Game/Library/Scenes", output.c_str(), save, size);
	ret = true;

	return ret;
}

uint ModuleSceneImporter::ImportPrefab(aiNode* node, uint mesh, uint material, std::string &output, const char* extension) const
{
	uint ret = NULL;

	if (mesh == NULL && material == NULL)
	{
		ret = NULL;
	}

	else
	{
		math::LCG random = math::LCG();

		uint uids[3] = {random.IntFast(), mesh, material};
		
		uint size = sizeof(uids);
		char* data = new char[size];

		memcpy(data, uids, size);

		char result_path[250];

		sprintf(result_path, "%s_prefab.%s", node->mName.C_Str(), extension);

		output = result_path;

		App->file_sys->SaveInDir("Game/Library/Prefab", result_path, data, size);
	}

	return ret;
}

uint ModuleSceneImporter::ImportMesh(const aiScene* scene, aiNode* node, std::string &output, const char* extension) const
{
	uint ret = NULL;

	if (node != nullptr)
	{

		if (node->mNumMeshes > 0)
		{
			
			for (uint i = 0; i < node->mNumMeshes; i++)
			{
		
				math::LCG random = math::LCG();
				uint index_scene = node->mMeshes[i];
				aiMesh* meshes = scene->mMeshes[index_scene];
				
				uint mesh_uid = ret = random.IntFast();
				//Storing number of indices, number of vertices, number of normals, number of texture coordinates (uvs)
				uint num_attributes[4] = { meshes->mNumFaces * 3, meshes->mNumVertices, NULL, NULL };
				
				if (meshes->HasNormals()) {
					num_attributes[2] = meshes->mNumVertices;
				}

				if (meshes->HasTextureCoords(0)) {
					num_attributes[3] = meshes->mNumVertices;
				}

				uint size = (sizeof(uint) + sizeof(num_attributes) + (sizeof(uint) * num_attributes[0]) + (sizeof(float) * num_attributes[1] * 3) + (sizeof(float) * num_attributes[2] * 3) + (sizeof(float) * num_attributes[3] * 2));
				
				char* data = new char[size];
				//Cursor is used to store each attribute in the correct place of the buffer "data"
				char* cursor = data;
				memcpy(cursor, &mesh_uid, sizeof(uint));
				cursor += sizeof(uint);

				uint bytes = sizeof(num_attributes);

				memcpy(cursor, num_attributes, bytes);
				cursor += bytes;

				uint* indices = new uint[num_attributes[0]];

				for (uint j = 0; j < meshes->mNumFaces; ++j)
				{
					if (meshes->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING! Geometry face with %d indices", meshes->mFaces[j].mNumIndices);
					}

					//Store the three indices of each face to mesh's array of indices
					else {
						memcpy(&indices[j * 3], meshes->mFaces[j].mIndices, 3 * sizeof(uint));
					}

				}

				memcpy(cursor, indices, sizeof(uint) * num_attributes[0]);
				cursor += sizeof(uint) * num_attributes[0];

				memcpy(cursor, meshes->mVertices, sizeof(float) * num_attributes[1] * 3);
				cursor += sizeof(float) * num_attributes[1] * 3;

				if(meshes->HasNormals())
				{
					memcpy(cursor, meshes->mNormals, sizeof(float) * num_attributes[2] * 3);
					cursor += sizeof(float) * num_attributes[2] * 3;
				}
				
				if (meshes->HasTextureCoords(0))
				{
					float* uvs = new float[num_attributes[3] * 2];
					aiVector3D* buff = meshes->mTextureCoords[0];

					//Copying x and y coordinates to its suitable uvs
					for (uint i = 0; i < (num_attributes[3] * 2) - 2; i = i + 2)
					{

						uvs[i] = buff->x;
						uvs[i + 1] = buff->y;

						buff++;
					}
					memcpy(cursor, uvs, sizeof(float) * num_attributes[3] * 2);
				}

				char result_path[250];

				sprintf(result_path, "%s.%s", node->mName.C_Str(), extension);

				output = result_path;

				if(App->resource_manager->IsMeshResource(output.c_str()) == false)
				{
					App->file_sys->SaveInDir("Game/Library/Mesh", result_path, data, size);
					App->resource_manager->ImportMesh(output.c_str(), ret);
					App->resource_manager->CreateMeshResource(ret, output.c_str());

				}
				else
				{
					ret = App->resource_manager->GetUidFromFile(output.c_str());
				}
			}
		}
	}
	return ret;
}

uint ModuleSceneImporter::ImportMaterial(const aiScene* scene, aiNode* node, std::string &output) const
{

	uint ret = NULL;

	if (scene->mNumMaterials > 0 && node->mNumMeshes > 0)
	{		
		
		uint material = scene->mMeshes[node->mMeshes[0]]->mMaterialIndex;

		aiMaterial* mat = scene->mMaterials[material];

		if (mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
			
			//So as to get the name of the texture, not relative to Fbx
			//Will append to mat_component->path, the last part of the texture path, its name
			char* buffer = new char[path.length + 1];
			strcpy(buffer, path.C_Str());

			for (uint i = 0; i < path.length + 1; i++)
			{
				if (buffer[i] == '\\')
				{
					for (uint j = 0; j <= i; j++)
					{
						buffer[j] = '*';
					}
				}
			}
			buffer[path.length] = '\0';

			while (buffer[0] == '*')
			{
				for (uint i = 0; i < path.length; i++)
				{
					buffer[i] = buffer[i + 1];
				}
			}

			std::string image_path;
			image_path.append("Game/Assets/Textures/");
			image_path.append(buffer);

			ret = ImportMaterial(image_path.c_str(), output);

			delete[] buffer;
			
		}

	}
		
	return ret;
}

uint ModuleSceneImporter::ImportMaterial(const char* directory, std::string &output) const
{
	char* buff = nullptr;
	uint ret = NULL;

	std::string image_path(directory);

	uint size = App->file_sys->Load(image_path.c_str(), &buff);

	std::string file_from_dir;
	App->file_sys->GetFileFromDir(directory, file_from_dir);

	const char* file = file_from_dir.c_str();

	ILuint id_image;

	ilGenImages(1, &id_image);
	ilBindImage(id_image);

	if (ilLoadL(IL_TYPE_UNKNOWN, buff, size))
	{
		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte* data;

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size = ilSaveL(IL_DDS, NULL, 0);

		if (size > 0)
		{
			data = new ILubyte[size];
			if (ilSaveL(IL_DDS, data, size) > 0)
			{
				char result_path[250];

				//sprintf(result_path, "%s.dds", data);

				char* image_name = new char[strlen(file) - 3];
				memcpy(image_name, file, strlen(file) - 4);

				image_name[strlen(file) - 4] = '\0';

				sprintf_s(result_path, 250, "%s.dds", image_name);

				output = result_path;

				App->file_sys->SaveInDir("Game/Library/Material", result_path, data, size);

				math::LCG random = math::LCG();
				ret = random.IntFast();


			}
			RELEASE(data);
		}
		ilDeleteImages(1, &id_image);
	}
	return ret;
}

bool ModuleSceneImporter::LoadTransform(aiNode* node, math::float3 &translation, math::Quat &rotation, math::float3 &scale) const
{
	aiMatrix4x4 transform = node->mTransformation;

	aiVector3D ai_scale;
	aiQuaterniont<float> ai_rotation;
	aiVector3D ai_position;

	transform.Decompose(ai_scale, ai_rotation, ai_position);

	scale.x = ai_scale.x;
	scale.y = ai_scale.y;
	scale.z = ai_scale.z;

	rotation.x = ai_rotation.x;
	rotation.y = ai_rotation.y;
	rotation.z = ai_rotation.z;
	rotation.w = ai_rotation.w;

	translation.x = ai_position.x;
	translation.y = ai_position.y;
	translation.z = ai_position.z;

	return true;
}
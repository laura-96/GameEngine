#include "Application.h"
#include "Globals.h"

#include "ModuleSceneImporter.h"

#include "ModuleFileSystem.h"

#include "cJSON.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

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
	App->scene_importer->ImportScene("Game/Assets/Town/Street.FBX");

	return  true;
}

bool ModuleSceneImporter::Save(cJSON* node)
{
	cJSON* strings = cJSON_GetObjectItem(node, "Imported scene");

	for (uint sc = 0; sc < scenes_imported.size(); sc++)
	{

		if (!strings)
		{
			cJSON_AddStringToObject(node, "Imported scene", scenes_imported[sc].c_str());
		}

		else
		{
			while (strcmp(strings->string, "Imported scene") == 0)
			{
				if (strcmp(strings->valuestring, scenes_imported[sc].c_str()) == 1)
				{
					cJSON_AddStringToObject(node, "Imported scene", scenes_imported[sc].c_str());
				}
			}
		}
		
	}

	//char* imp = cJSON_Print(importer);
	//cJSON_Parse(imp);

	return true;
}

bool ModuleSceneImporter::ImportScene(const char* file)
{

	bool ret = true;

	for (uint sc = 0; sc < scenes_imported.size(); sc++)
	{
		if (strcmp(scenes_imported[sc].c_str(), file) == 0)
		{
			ret = false;
		}
	}

	if (ret)
	{
		std::string scene_name = file;

		scenes_imported.push_back(scene_name);
		math::LCG random = math::LCG();

		const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene != nullptr && scene->mRootNode != nullptr && scene->HasMeshes())
		{
			aiNode* node = scene->mRootNode;
			std::vector<aiNode*> closed_nodes;

			std::string output;
			std::string output_image;

			ImportMesh(scene, node, output, "lau");
			ImportMaterial(scene, node, output_image);

			uint uid = random.IntFast();
			ObjectImport* root_obj = new ObjectImport(uid, NULL, output, output_image);

			LoadTransform(node, root_obj->translation, root_obj->rotation, root_obj->scale);
			obj_import.push_back(root_obj);

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

						ImportMesh(scene, node, output, "lau");
						ImportMaterial(scene, node, output_image);

						uint uid = random.IntFast();
						ObjectImport* root_obj = new ObjectImport(uid, NULL, output, output_image);

						LoadTransform(node, root_obj->translation, root_obj->rotation, root_obj->scale);
						obj_import.push_back(root_obj);

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
						node = node->mParent;
					}
				}
			}
			closed_nodes.clear();
		}

		else
		{
			ret = false;
			LOG("Error loading scene: %s", file);
		}

		aiReleaseImport(scene);
	}

	return ret;
}

bool ModuleSceneImporter::ImportMesh(const aiScene* scene, aiNode* node, std::string &output, const char* extension) const
{
	bool ret = false;

	if (node != nullptr)
	{
		//cJSON* component = cJSON_CreateObject();

		//cJSON_AddItemToObject(write_info, "Components", component);
		ret = true;

		if (node->mNumMeshes > 0)
		{
			
			for (uint i = 0; i < node->mNumMeshes; i++)
			{
		
				uint index_scene = node->mMeshes[i];
				aiMesh* meshes = scene->mMeshes[index_scene];
				
				//Storing number of indices, number of vertices, number of normals, number of texture coordinates (uvs)
				uint num_attributes[4] = { meshes->mNumFaces * 3, meshes->mNumVertices, NULL, NULL };
				
				if (meshes->HasNormals()) {
					num_attributes[2] = meshes->mNumVertices;
				}

				if (meshes->HasTextureCoords(0)) {
					num_attributes[3] = meshes->mNumVertices;
				}

				uint size = (sizeof(num_attributes) + (sizeof(uint) * num_attributes[0]) + (sizeof(float) * num_attributes[1] * 3) + (sizeof(float) * num_attributes[2] * 3) + (sizeof(float) * num_attributes[3] * 2));
				
				char* data = new char[size];
				//Cursor is used to store each attribute in the correct place of the buffer "data"
				char* cursor = data;
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
					cursor += num_attributes[2] * 3;
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

				App->file_sys->SaveInDir("Game/Library/Mesh",result_path, data, size);
			}
		}
	}
	return ret;
}

bool ModuleSceneImporter::ImportMaterial(const aiScene* scene, aiNode* node, std::string &output) const
{

	bool ret = false;
	char* buff = nullptr;

	if (scene->mNumMaterials > 0 && node->mNumMeshes > 0)
	{		
		std::string image_path;

		image_path.clear();
		image_path.append("Game/Assets/Textures/");

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

			image_path.append(buffer);

			uint size = App->file_sys->Load(image_path.c_str(), &buff);

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

						sprintf(result_path, "%s.dds", data);

						output = result_path;

						char* image_name = new char[strlen(buffer) - 3];
						memcpy(image_name, buffer, strlen(buffer) - 4);
						
						image_name[strlen(buffer) - 4] = '\0';

						sprintf_s(result_path, 250, "%s.dds", image_name);

						ret = App->file_sys->SaveInDir("Game/Library/Material", result_path, data, size);
					}
					RELEASE(data);
				}
				ilDeleteImages(1, &id_image);
			}


			delete[] buffer;
			
		}

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


	return true;
}
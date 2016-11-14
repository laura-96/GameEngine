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
	

	return true;
}

bool ModuleSceneImporter::ImportScene(const char* file)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->mRootNode != nullptr && scene->HasMeshes())
	{
		aiNode* node = scene->mRootNode;
		std::vector<aiNode*> closed_nodes;
		std::string output;
		std::string output_image;
		std::string output_prefab;

		uint im_uid = ImportMesh(scene, node, output, "lau");
		uint mat_uid = ImportMaterial(scene, node, output_image);
		ImportPrefab(node, im_uid, mat_uid, output_prefab, "gl");
		
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
					ImportPrefab(node, im_uid, mat_uid, output_prefab, "gl");


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

	return ret;
}

bool ModuleSceneImporter::ImportPrefab(aiNode* node, uint mesh, uint material, std::string &output, const char* extension) const
{
	bool ret = true;

	if (mesh == NULL && material == NULL)
	{
		ret = false;
	}

	else
	{
		uint size = sizeof(uint) + sizeof(uint) + sizeof(uint);
		char* data = new char[size];

		char* cursor = data;
		math::LCG random = math::LCG();

		uint ran = random.IntFast();

		memcpy(cursor, &ran, sizeof(uint));
		cursor += sizeof(uint);

		memcpy(cursor, &mesh, sizeof(uint));
		cursor += sizeof(uint);

		memcpy(cursor, &material, sizeof(uint));
		cursor += sizeof(uint);

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

				App->file_sys->SaveInDir("Game/Library/Mesh", result_path, data, size);
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

			ret = ImportMaterial(buffer, output);

			delete[] buffer;
			
		}

	}
		
	return ret;
}

uint ModuleSceneImporter::ImportMaterial(const char* file, std::string &output) const
{
	char* buff = nullptr;
	uint ret = NULL;

	std::string image_path;
	image_path.append("Game/Assets/Textures/");
	image_path.append(file);
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

				char* image_name = new char[strlen(file) - 3];
				memcpy(image_name, file, strlen(file) - 4);

				image_name[strlen(file) - 4] = '\0';

				sprintf_s(result_path, 250, "%s.dds", image_name);

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


	return true;
}
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

	importer = node;
	return true;
}

bool ModuleSceneImporter::Start()
{


	return  true;
}

bool ModuleSceneImporter::ImportScene(const char* file)
{
	bool ret = true;
	math::LCG random = math::LCG();

	cJSON* scene_json = cJSON_CreateObject();
	cJSON_AddItemToObject(importer, file, scene_json);

	const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->mRootNode != nullptr && scene->HasMeshes())
	{
		aiNode* node = scene->mRootNode;
		std::vector<aiNode*> closed_nodes;

		cJSON* go = cJSON_CreateObject();
		cJSON_AddItemToObject(scene_json, "Game Object - Root", go);

		cJSON_AddNumberToObject(go, "UID", random.IntFast());
		cJSON_AddNumberToObject(go, "Parent UID", NULL);
		
		/*IMPORT COMPONENTS
		if (!LoadComponents(scene, node, root_GO))
		{
			ret = false;
		}*/
		uint parent_uid = cJSON_GetObjectItem(go, "UID")->valueint;

		std::string output_file;
		ImportMesh(scene, node, parent_uid, output_file, "lau");

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
					
					go = cJSON_CreateObject();
					cJSON_AddItemToObject(scene_json, "Game Object - Root", go);

					cJSON_AddNumberToObject(go, "UID", random.IntFast());
					cJSON_AddNumberToObject(go, "Parent UID", parent_uid);

					parent_uid = cJSON_GetObjectItem(go, "UID")->valueint;

					std::string output;
					ImportMesh(scene, node, parent_uid, output, "lau");

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
					parent_uid = cJSON_GetObjectItem(go, "Parent UID")->valueint;
					
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

bool ModuleSceneImporter::ImportMesh(const aiScene* scene, aiNode* node, uint UID, std::string &output, const char* extension) const
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

				uint size = (sizeof(num_attributes) + (sizeof(uint) * num_attributes[0]) + (sizeof(float) * num_attributes[1]) + (sizeof(float) * num_attributes[2]) + (sizeof(float) * num_attributes[3]));
				
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
					memcpy(cursor, uvs, num_attributes[3] * 2);
				}

				char result_path[250];

				sprintf(result_path, "%s_%u.%s", meshes->mName.C_Str(), UID, extension);

				output = result_path;

				App->file_sys->Save(result_path, data, size);
			}
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
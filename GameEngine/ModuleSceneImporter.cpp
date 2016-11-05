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

					/*/IMPORT COMPONENTS
					if (!LoadComponents(scene, node, new_go))
					{
						ret = false;
					}*/

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
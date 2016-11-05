#include "Application.h"
#include "Globals.h"

#include "ModuleSceneImporter.h"

#include "ModuleFileSystem.h"
#include "cJSON.h"

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
	cJSON* importer = cJSON_CreateObject();
	cJSON_AddItemToObject(node, "Importer", importer);

	cJSON_AddStringToObject(importer, "String", "Hello world");

	char* str = cJSON_GetObjectItem(node, "Importer")->child->valuestring;

	return true;
}

bool ModuleSceneImporter::Start()
{
	
	return  true;
}

bool ModuleSceneImporter::ImportScene(const char* file, std::string &output, const char* extension)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->mRootNode != nullptr && scene->HasMeshes())
	{
		aiNode* node = scene->mRootNode;
		std::vector<aiNode*> closed_nodes;
		uint size = sizeof(GO_reference);


		go_importer->name = node->mName.C_Str();
		go_importer->num_children = node->mNumChildren;
		
		size += (sizeof(go_importer->name) + sizeof(uint) + sizeof(std::vector<std::string>));

		for (int i = 0; i < node->mNumChildren; i++)
		{
			go_importer->children_names.push_back(node->mChildren[i]->mName.C_Str());
			size += (sizeof(go_importer->children_names[i]));
		}
		
		/*IMPORT COMPONENTS
		if (!LoadComponents(scene, node, root_GO))
		{
			ret = false;
		}*/
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
					
					go_importer->name = node->mName.C_Str();
					go_importer->num_children = node->mNumChildren;

					size += (sizeof(go_importer->name) + sizeof(uint) + sizeof(std::vector<std::string>));

					for (int i = 0; i < node->mNumChildren; i++)
					{
						go_importer->children_names.push_back(node->mChildren[i]->mName.C_Str());
						size += (sizeof(go_importer->children_names[i]));
					}
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
					node = node->mParent;
				}
			}
		}
		closed_nodes.clear();
		char* data = new char[size];
		
		App->file_sys->Save(output.c_str(), data, size);
	}

	else
	{
		ret = false;
		LOG("Error loading scene: %s", file);
	}

	aiReleaseImport(scene);

	return ret;
}